
//******************************************************************************//
// INFO                                                                         //
//******************************************************************************//
// File            : main.c                                                     //
// Author          : Aditya Mall                                                //
// Date            : 02/05/2019                                                 //
// Copyright       : (c) 2019, Aditya Mall, Mentor: Dr. Jason Losh,             //
//                   The University of Texas at Arlington.                      //
// Project         : RTOS Framework EK-TM4C123GXL Evaluation Board.             //
// Target Platform : EK-TM4C123GXL Evaluation Board                             //
// Target uC       : TM4C123GH6PM                                               //
// IDE             : Code Composer Studio v7                                    //
// System Clock    : 40 MHz                                                     //
// UART Baudrate   : 115200                                                     //
// Data Length     : 8 Bits                                                     //
// Version         : 1.5                                                        //
//                                                                              //
// Hardware configuration:                                                      //
//                                                                              //
// (On Board)                                                                   //
//  - Red LED at PF1 drives an NPN transistor that powers the red LED           //
//  - Blue LED at PF2 drives an NPN transistor that powers the blue LED         //
//  - Green LED at PF3 drives an NPN transistor that powers the green LED       //
//  - Pushbutton at SW1 pulls pin PF4 low (internal pull-up is used)            //
//  - UART Interface:                                                           //
//       U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller          //
//       Configured to 115,200 baud, 8N1                                        //
//                                                                              //
// (External Modules)                                                           //
//  -  5 Pushbuttons and 5 LEDs, UART                                           //
//  -  LEDS on these pins:                                                      //
//  -  Blue:   PF2 (on-board)                                                   //
//  -  Red:    PE1                                                              //
//  -  Green:  PE2                                                              //
//  -  Yellow: PE3                                                              //
//  -  Orange: PE4                                                              //
//  -  PB0:    PA2                                                              //
//  -  PB1:    PA3                                                              //
//  -  PB2:    PA4                                                              //
//  -  PB3:    PA6                                                              //
//  -  PB4:    PA7                                                              //
//                                                                              //
//                                                                              //
//******************************************************************************//
// ATTENTION                                                                    //
//******************************************************************************//
//                                                                              //
// This Software was made by Aditya Mall, under the guidance of Dr. Jason Losh, //
// The University of Texas at Arlington. Any UNAUTHORIZED use of this software, //
// without the prior permission and consent of Dr. Jason Losh or any of the,    //
// mentioned contributors is a direct violation of Copyright.                   //
//                                                                              //
// THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED   //
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF           //
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. //
// ADITYA MALL OR ANY MENTIONED CONTRIBUTORS SHALL NOT, IN ANY CIRCUMSTANCES,   //
// BE LIABLE FOR SPECIAL, INCIDENTAL,OR CONSEQUENTIAL DAMAGES,                  //
// FOR ANY REASON WHATSOEVER.                                                   //
//                                                                              //
// For more info please contact: aditya.mall@mavs.uta.edu                       //
//                                                                              //
//******************************************************************************//


//***** References and Citations ******//
//
// 1) ANSI VT100 escape sequence sourced from: "http://ascii-table.com/ansi-escape-sequences-vt-100.php"
// 2) VT100 Operating system control sequences sourced from "http://rtfm.etla.org/xterm/ctlseq.html"
// 3) List of Unicode Characters "https://en.wikipedia.org/wiki/List_of_Unicode_characters"
//
//*************************************//


//*********** Versions ***************//
//
// Version 1.5-(02/23/2018)
// info:
//      - Step 4, 5, 6 and 7 added,
//      - svc offset changes #48, (queue struct member is used in processQueue array)
//
// Version 1.4-(02/09/3019)
// info:
///     - Step 1 Added, setStackPt and getStackPt
//      - Step 2 and 3 Added, yield implementation and pendsvc call
//
// Version 1.3-(02/05/2019)
// info:
//      - External module / hardware specific pin initializations added for 5 push buttons and Leds
//      - Test function added for testing the external modules on daughter board
//      - Structure defined for control of test function
//      - echo command implemented
//
// Version 1.2-(01/26/2019)
// info:
//      - command_line function replaces term_getsUart0 function, with backspace bug removed
//
// Version 1.1-(01/21/2019)
// info:
//      - added functions to copy string from source to destination buffer
//      - uSTRCPY, uSTRCMP and uSTRLEN, custom implementations of string.h library functions
//      - string.h library removed
//      - stack size reduced from 8K to 1024 bytes
//
// Version 1.0-(01/20/2019)
// info:
//      - added user string length and string compare functions
//
//************************************//


//*****************************************************************************//
//                                                                             //
//              STANDARD LIBRARIES AND BOARD SPECIFIC HEADER FILES             //
//                                                                             //
//*****************************************************************************//

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"



//*****************************************************************************//
//                                                                             //
//               RTOS Defines and Kernel Variables                             //
//                                                                             //
//*****************************************************************************//

// function pointer
typedef void (*_fn)();


// prototypes
uint8_t get_svcValue(void);
void setStackPt(void* func_stack);
uint32_t* getStackPt();

// semaphore
#define MAX_SEMAPHORES 5
#define MAX_QUEUE_SIZE 5

struct semaphore
{
    uint16_t count;
    uint16_t queueSize;
    uint32_t processQueue[MAX_QUEUE_SIZE]; // store task index here

} semaphores[MAX_SEMAPHORES];

uint8_t semaphoreCount = 0;
struct semaphore *keyPressed, *keyReleased, *flashReq, *resource, *SemaphorePt;

// task
#define STATE_INVALID    0           // no task
#define STATE_UNRUN      1           // task has never been run
#define STATE_READY      2           // has run, can resume at any time
#define STATE_DELAYED    3           // has run, but now awaiting timer
#define STATE_BLOCKED    4           // has run, but now blocked by semaphore

#define MAX_TASKS        10          // maximum number of valid tasks
#define CLOCKFREQ        40000000    // Main Clock/BUS Frequency
#define SYSTICKFREQ      1000        // Systic timer Frequency

uint8_t taskCurrent = 0;             // index of last dispatched task
uint8_t taskCount = 0;               // total number of valid tasks

uint32_t stack[MAX_TASKS][256];      // 1024 byte stack for each thread

struct _tcb
{
    uint8_t state;                   // see STATE_ values above
    void *pid;                       // used to uniquely identify thread
    void *sp;                        // location of stack pointer for thread
    int8_t priority;                 // -8=highest to 7=lowest
    int8_t currentPriority;          // used for priority inheritance
    uint32_t ticks;                  // ticks until sleep complete
    char name[16];                   // name of task used in ps command
    void *semaphore;                 // pointer to the semaphore that is blocking the thread
} tcb[MAX_TASKS];


enum svc_cases
{
    svcYIELD = 100,                  // Value of yield label in switch case
    svcSLEEP = 101,                  // Value of switch label in switch case
    svcWAIT  = 102,                  // Value of wait label in switch case
    svcPOST  = 103,                  // Value of post label in switch case
};

uint32_t* SystemStackPt;             // Pointer to the Main Stack pointer
uint8_t svc_value;                   // Value of service call by SVC instruction


//*****************************************************************************//
//                                                                             //
//                MACRO DEFINITIONS, DIRECTIVES and STRUCTURES                 //
//                                                                             //
//*****************************************************************************//


//*******************Debug and Code test defines**********************//
#ifndef DEBUG
#define DEBUG
#endif

#ifndef TEST
#define TEST
#endif

#ifndef EXP
//#define EXP
#endif


//****************** Bit Banding defines for Pins *********************//

//PORT E
#define PE1               (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 1*4)))
#define PE2               (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 2*4)))
#define PE3               (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 3*4)))
#define PE4               (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 4*4)))

//Port F
#define PF1               (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))
#define PF2               (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4)))
#define PF3               (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))
#define PF4               (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 4*4)))

//Port A
#define PA2               (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 2*4)))
#define PA3               (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 3*4)))
#define PA4               (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 4*4)))
#define PA5               (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 5*4)))
#define PA6               (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 6*4)))
#define PA7               (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 7*4)))



//***************************** Board Modules Pins *************************//


#define ONBOARD_RED_LED           PF1
#define ONBOARD_BLUE_LED          PF2
#define ONBOARD_GREEN_LED         PF3
#define ONBOARD_PUSH_BUTTON       PF4

//************************ External Modules Pins ***************************//

#define RED_LED                   PE1
#define BLUE_LED                  ONBOARD_BLUE_LED
#define GREEN_LED                 PE2
#define YELLOW_LED                PE3
#define ORANGE_LED                PE4

#define PB0                       PA2
#define PB1                       PA3
#define PB2                       PA4
#define PB3                       PA5
#define PB4                       PA6

#define BUTTON_L1                 PB4
#define BUTTON_L2                 PB3

#define BUTTON_R1                 PB2
#define BUTTON_R2                 PB1
#define BUTTON_R3                 PB0

//************************** Project Specific Defines **********************//

#define MAX_SIZE                  80
#define MAX_ARGS                  80

#define DELIMS                    ( (string[i] >= 33 && string[i] <= 44) || \
                                    (string[i] >= 46 && string[i] <= 47) || \
                                    (string[i] >= 58 && string[i] <= 64) || \
                                    (string[i] >= 91 && string[i] <= 96) )


#define ARGS_CHECK(num)           (args_updated < num || args_updated > num)


//*************************** Structs ************************************//

typedef struct test_modes
{
    uint8_t commands;
    uint8_t onboard_test;
    uint8_t external_hw_test;

}test_modes_type;


//*****************************************************************************//
//                                                                             //
//                          Function Prototypes                                //
//                                                                             //
//*****************************************************************************//

//Delay and Blocking Functions
uint8_t waitPbPress(void);
void waitMicrosecond(uint32_t us);


//UART IO Control functions
void putcUart0(const char c);
void putsUart0(const char* str);
char getcUart0(void);
void putnUart0(uint32_t Number);
void clear_screen(void);
void command_line(void);


//String functions
uint8_t uSTRCMP(char *string_1, char *string_2);
uint8_t uSTRLEN(const char *string);
void uSTRCPY(char *string_dest, char *string_src);
void parse_string(void);
int8_t is_command(char* command, uint8_t arg);


// test command function
void test_commands(void);
void extern_mods_test_commands(void);


//Project Command Functions
void project_info(void);
void TIVA_shell(void);


//Buffer Reset Control Functions
void reset_buffer(void);
void reset_new_string(void);


//*****************************************************************************//
//                                                                             //
//                     USER GLOBAL VARIABLES                                   //
//                                                                             //
//*****************************************************************************//


// String variables
char string[MAX_SIZE]               = {0};                                         // Array to store the chars received from UART
char new_string[MAX_ARGS][MAX_SIZE] = {0};                                         // Array to store the words after dividing the string to tokens
char buff_int[MAX_SIZE]             = {0};                                         // Array buffer to store converted value of integer to char

// Char category variables
uint8_t a[MAX_ARGS] = {0};                                                         // Array to store the record of Alpha characters
uint8_t n[MAX_ARGS] = {0};                                                         // Array to store the record of Numeric characters
uint8_t s[MAX_ARGS] = {0};                                                         // Array to store the record of Special characters


// Argument count variables
uint8_t args_no      = 0;                                                          // Variable for indexing initial number of arguments
uint8_t args_str     = 0;                                                          // Variable for indexing the number of characters per argument
uint8_t args_updated = 0;                                                          // Variable for indexing final number of arguments, not initialized to zero

// Test Variables
//int char_count =0;

// Struct variables
test_modes_type testMode;
int char_count = 0;


//-----------------------------------------------------------------------------
// RTOS Kernel Functions
//-----------------------------------------------------------------------------

void rtosInit()
{
    uint8_t i;
    // no tasks running
    taskCount = 0;
    // clear out tcb records
    for (i = 0; i < MAX_TASKS; i++)
    {
        tcb[i].state = STATE_INVALID;
        tcb[i].pid = 0;
    }

    // REQUIRED: initialize systick for 1ms system timer
    NVIC_ST_CTRL_R     = 0;                      // Clear Control bit for safe programming
    NVIC_ST_CURRENT_R  = 0;                      // Start Value
    NVIC_ST_RELOAD_R   = 40000;                  // Set for 1Khz
    NVIC_ST_CTRL_R     = 7;                      // set for source as clock interrupt enable and enable the timer.
}

// REQUIRED: Implement prioritization to 8 levels
int rtosScheduler()
{
    bool ok;
    static uint8_t task = 0xFF;
    ok = false;
    while (!ok)
    {
        task++;
        if (task >= MAX_TASKS)
            task = 0;
        ok = (tcb[task].state == STATE_READY || tcb[task].state == STATE_UNRUN);
    }
    return task;
}


uint32_t* tcb_stackPT;

uint32_t* updated_stackPT;



void rtosStart()
{
    // REQUIRED: add code to call the first task to be run
    _fn fn;

    // Add code to initialize the SP with tcb[task_current].sp;
    SystemStackPt  = getStackPt();

    taskCurrent = rtosScheduler();

    //tcb_stackPT = tcb[taskCurrent].sp;

    setStackPt(tcb[taskCurrent].sp);

    //updated_stackPT = getStackPt();

    fn = (_fn)tcb[taskCurrent].pid;

    tcb[taskCurrent].state = STATE_READY;
    (*fn)();

}

bool createThread(_fn fn, char name[], int priority)
{
    bool ok = false;
    uint8_t i = 0;
    bool found = false;
    // REQUIRED: store the thread name
    // add task if room in task list
    if (taskCount < MAX_TASKS)
    {
        // make sure fn not already in list (prevent reentrancy)
        while (!found && (i < MAX_TASKS))
        {
            found = (tcb[i++].pid ==  fn);
        }
        if (!found)
        {
            // find first available tcb record
            i = 0;
            while (tcb[i].state != STATE_INVALID) {i++;}
            tcb[i].state = STATE_UNRUN;
            tcb[i].pid = fn;
            tcb[i].sp = &stack[i][255];
            tcb[i].priority = priority;
            tcb[i].currentPriority = priority;
            // increment task count
            taskCount++;
            ok = true;
        }
    }
    // REQUIRED: allow tasks switches again
    return ok;
}

//REQUIRED: modify this function to destroy a thread
// REQUIRED: remove any pending semaphore waiting
void destroyThread(_fn fn)
{
}

// REQUIRED: modify this function to set a thread priority
void setThreadPriority(_fn fn, uint8_t priority)
{
}

struct semaphore* createSemaphore(uint8_t count)
{
    struct semaphore *pSemaphore = 0;
    if (semaphoreCount < MAX_SEMAPHORES)
    {
        pSemaphore = &semaphores[semaphoreCount++];
        pSemaphore->count = count;
    }
    return pSemaphore;
}

// REQUIRED: modify this function to yield execution back to scheduler using pendsv
// push registers, call scheduler, pop registers, return to new function
void yield()
{
    //putsUart0("yield\r\n");
    __asm(" SVC #100");

}

// REQUIRED: modify this function to support 1ms system timer
// execution yielded back to scheduler until time elapses using pendsv
// push registers, set state to delayed, store timeout, call scheduler, pop registers,
// return to new function (separate unrun or ready processing)
void sleep(uint32_t tick)
{
    __asm(" SVC #101");
}

// REQUIRED: modify this function to wait a semaphore with priority inheritance
// return if avail (separate unrun or ready processing), else yield to scheduler using pendsv
void wait(struct semaphore *pSemaphore)
{
    __asm(" SVC #102");
}

// REQUIRED: modify this function to signal a semaphore is available using pendsv
void post(struct semaphore *pSemaphore)
{
    __asm(" SVC #103");
}

// REQUIRED: modify this function to add support for the system timer
// REQUIRED: in preemptive code, add code to request task switch
void systickIsr()
{
    uint32_t i;

    for(i=0; i < MAX_TASKS; i++)
    {
        if(tcb[i].state == STATE_DELAYED)
        {
            tcb[i].ticks--;

            if(tcb[i].ticks == 0)
                tcb[i].state = STATE_READY;
        }

    }

}

uint32_t* PC_VAL = 0;

// REQUIRED: in coop and preemptive, modify this function to add support for task switching
// REQUIRED: process UNRUN and READY tasks differently
void pendSvIsr(void)
{
    __asm(" PUSH {R4-R11}");                      // Push reg list
    __asm(" MOV R4,LR");


    tcb[taskCurrent].sp = getStackPt();           // save stack pointer in tcb
    setStackPt(SystemStackPt);                    // set stack pointer to System Stack pointer
    taskCurrent = rtosScheduler();                // task current = rtos scheduler


    if(tcb[taskCurrent].state == STATE_READY)
    {
        setStackPt(tcb[taskCurrent].sp);

        __asm(" POP {R4-R11}");

    }
    else if(tcb[taskCurrent].state == STATE_UNRUN)// unrun
    {
        tcb[taskCurrent].state = STATE_READY;
        setStackPt(tcb[taskCurrent].sp);

        __asm(" MOV R0, #0x01000000" );        //0x01000000
        __asm(" PUSH {R0}"           );
        PC_VAL = tcb[taskCurrent].pid;
        __asm(" PUSH {R0}"           );
        __asm(" PUSH {LR}"           );
        __asm(" PUSH {R12}"          );
        __asm(" PUSH {R0-R3}"        );

        __asm(" PUSH {R4}"           );    //value of LR
        __asm(" PUSH {R3}"           );

    }

}


uint8_t get_svcValue(void)
{
    __asm(" MOV  R0, SP"   );
    __asm(" ADD  R0, #48"  );
    __asm(" LDR  R0, [R0]" );
    __asm(" SUBS R0, #2"   );
    __asm(" LDR  R0, [R0]" );
    __asm(" BX   LR"       );

    return 0;
}

uint32_t ret_R0(void)
{
    // Empty function for returning the value of R0
    __asm(" BX LR"    );

    return 0;
}

uint32_t ret_R1(void)
{
    __asm(" MOV R0,R1" );
    __asm(" BX LR"     );

    return 0;
}

uint32_t ret_R2(void)
{
    __asm(" MOV R0,R2" );
    __asm(" BX LR"     );

    return 0;
}


// REQUIRED: modify this function to add support for the service call
// REQUIRED: in preemptive code, add code to handle synchronization primitives
void svCallIsr(void)
{
    uint32_t R0 = ret_R0();                                                   // Get value of r0
    uint32_t R1 = ret_R1();                                                   // Get value of r1
    uint32_t R2 = ret_R2();                                                   // Get value of r2

    svc_value = get_svcValue();                                               // Get SVC value

    switch(svc_value)
    {

    case svcYIELD:
                  tcb[taskCurrent].state = STATE_READY;                       // Set task ready

                  NVIC_INT_CTRL_R= NVIC_INT_CTRL_PEND_SV;                     // Set pendsv bit
                  break;


    case svcSLEEP:
                  tcb[taskCurrent].ticks = R0;                                // Set sleep timeout value
                  tcb[taskCurrent].state = STATE_DELAYED;                     // Set state as delayed, it can't be scheduled till the time it is not in ready state

                  NVIC_INT_CTRL_R= NVIC_INT_CTRL_PEND_SV;                     // Set pendsv bit
                  break;

    case svcWAIT:
                  SemaphorePt = (struct semaphore*)R0;                        // Get the pointer to the semaphore

                  if(SemaphorePt->count > 0)                                  // Check for value of sem count variable
                  {
                      SemaphorePt->count--;                                   // Decrement the count if count > 0
                  }
                  else
                  {
                      SemaphorePt->processQueue[SemaphorePt->queueSize] =     // Store task in sem process queue
                                           (uint32_t)tcb[taskCurrent].pid;

                      SemaphorePt->queueSize++;                               // Increment the index of the queue for next task
                      tcb[taskCurrent].state     = STATE_BLOCKED;             // Mark the state of of current task as blocked
                      tcb[taskCurrent].semaphore = SemaphorePt;               // Store the pointer to semaphore

                      NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;               // Set pendsv Inside 'else' since we don't have switch task all the time
                  }
                  break;

    case svcPOST:
                 SemaphorePt = (struct semaphore*)R0;                         // Get Pointer to the semaphore
                 SemaphorePt->count++;                                        // Increment the count


    default:                                                                  // Used for Debugging
                  putsUart0(__FUNCTION__);
                  putcUart0(':');
                  putsUart0("Entered 'Default' of switch case\r\n");
                  break;

    }

}


void setStackPt(void* func_stack)
{

    __asm(" MOV SP, R0"    );
    __asm(" BX LR"         );

}

uint32_t* getStackPt()
{

    __asm(" MOV R0,SP"     );
    __asm(" BX  LR"        );

    return 0;
}


//------------------------------------------------------------------------------
//  Task functions
// ------------------------------------------------------------------------------

// one task must be ready at all times or the scheduler will fail
// the idle task is implemented for this purpose
void idle(void)
{
    while(true)
    {
        ORANGE_LED = 1;
        waitMicrosecond(1000000);
        ORANGE_LED = 0;
        yield();
    }
}

//void idle2(void)
//{
//    while(true)
//    {
//        YELLOW_LED = 1;
//        waitMicrosecond(1000000);
//        YELLOW_LED = 0;
//        yield();
//    }
//}

void flash4Hz(void)
{
    while(true)
    {
        GREEN_LED ^= 1;
        sleep(125);
    }
}


void oneshot()
{
    while(true)
    {
        wait(flashReq);
        YELLOW_LED = 1;
        sleep(1000);
        YELLOW_LED = 0;
    }
}

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

uint8_t readPbs()
{
    uint8_t retval_button = 0;

    waitMicrosecond(50000);
    if(!PB0)
    {
        retval_button |= 1;
    }

    if(!PB1)
    {
        retval_button |= 2;
    }

    if(!PB2)
    {
        retval_button |= 4;
    }

    if(!PB3)
    {
        retval_button |= 8;
    }

    if(!PB4)
    {
        retval_button |= 16;
    }

    if(!ONBOARD_PUSH_BUTTON)
    {
        testMode.external_hw_test = 0;
        //carriage_return = 1;
    }


    return retval_button;
}
//*****************************************************************************//
//                                                                             //
//                     HARDWARE INTIALIZATION FUNCTION                         //
//                                                                             //
//*****************************************************************************//

void initHw()
{
    //******************************************************* Clock Configs ******************************************************************//

    // Configure System clock as 40Mhz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (0x04 << SYSCTL_RCC_SYSDIV_S);

    // Enable GPIO port A, and F peripherals
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOF | SYSCTL_RCGC2_GPIOE;



    //**************************************************** On Board Modules ******************************************************************//

    // Configure On boards RED, GREEN and BLUE led and Pushbutton Pins
    GPIO_PORTF_DEN_R |= (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);                 // Enable Digital
    GPIO_PORTF_DIR_R |= (1 << 1) | (1 << 2) | (1 << 3);                            // Enable as Output
    GPIO_PORTF_DIR_R &= ~(0x10);                                                   // Enable push button as Input
    GPIO_PORTF_PUR_R |= 0x10;                                                      // Enable internal pull-up for push button


    //********************************************** Console IO Hardware Configs *************************************************************//

    // Configure UART0 pins
    SYSCTL_RCGCUART_R  |= SYSCTL_RCGCUART_R0;                                      // Turn-on UART0, leave other uarts in same status
    GPIO_PORTA_DEN_R   |= 3;                                                       // Turn on Digital Operations on PA0 and PA1
    GPIO_PORTA_AFSEL_R |= 3;                                                       // Select Alternate Functionality on PA0 and PA1
    GPIO_PORTA_PCTL_R  |= GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX;                 // Select UART0 Module

    // Configure UART0 to 115200 baud, 8N1 format (must be 3 clocks from clock enable and config writes)
    UART0_CTL_R   = 0;                                                             // turn-off UART0 to allow safe programming
    UART0_CC_R   |= UART_CC_CS_SYSCLK;                                             // use system clock (40 MHz)
    UART0_IBRD_R  = 21;                                                            // r = 40 MHz / (Nx115.2kHz), set floor(r)=21, where N=16
    UART0_FBRD_R  = 45;                                                            // round(fract(r)*64)=45
    UART0_LCRH_R |= UART_LCRH_WLEN_8 | UART_LCRH_FEN;                              // configure for 8N1 w/ 16-level FIFO
    UART0_CTL_R  |= UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;                 // enable TX, RX, and module


    //***************************************************** External Modules ******************************************************************//

    // External Push Buttons
    GPIO_PORTA_DEN_R |= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6);
    GPIO_PORTA_DIR_R &= ~(1 << 2) | ~(1 << 3) | ~(1 << 4) | ~(1 << 5) | ~(1 << 6);
    GPIO_PORTA_PUR_R |= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6);

    // External LEDs
    GPIO_PORTE_DEN_R |= (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);
    GPIO_PORTE_DIR_R |= (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);

}

//*****************************************************************************//
//                                                                             //
//                          DELAY FUNCTIONS                                    //
//                                                                             //
//*****************************************************************************//



//micro second delay function
void waitMicrosecond(uint32_t us)
{
    __asm("WMS_LOOP0:   MOV  R1, #6"       );
    __asm("WMS_LOOP1:   SUB  R1, #1"       );
    __asm("             CBZ  R1, WMS_DONE1");
    __asm("             NOP"               );
    __asm("             NOP"               );
    __asm("             B    WMS_LOOP1"    );
    __asm("WMS_DONE1:   SUB  R0, #1"       );
    __asm("             CBZ  R0, WMS_DONE0");
    __asm("             NOP"               );
    __asm("             B    WMS_LOOP0"    );
    __asm("WMS_DONE0:"                     );
}

// Blocking function that returns only when SW1 is pressed
uint8_t waitPbPress(void)
{
    if (ONBOARD_PUSH_BUTTON)
        return 0;

    else
    {
        waitMicrosecond(50000);
        return 1;
    }
}


//*****************************************************************************//
//                                                                             //
//                     UART IO Control Functions                               //
//                                                                             //
//*****************************************************************************//


// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(const char c)
{
    while (UART0_FR_R & UART_FR_TXFF);
    UART0_DR_R = c;
}

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(const char* str)
{
    uint8_t i;

    for (i = 0; i < uSTRLEN(str); i++)
        putcUart0(str[i]);
}

// Blocking function that returns with serial data once the buffer is not empty
char getcUart0(void)
{
    while (UART0_FR_R & UART_FR_RXFE);
    return UART0_DR_R & 0xFF;
}

// Blocking Function for getting the input as string once the buffer is not empty
void getsUart0(void)
{
    char input;

    while(1)
    {
        input = getcUart0();
        putcUart0(input);
    }

}

//blocking function for integer print
void putnUart0(uint32_t Number)
{
    char NumBuff[15] = {0};

    uint32_t Remainder = 0;
    uint32_t TempVar   = 0;
    uint32_t Count     = 0;
    uint32_t Digits    = 0;
    uint32_t i         = 0;

    // Store Number in Temporary Variable
    TempVar = Number;

    // Count the number of digits
    while(1)
    {
        Number = Number / 10;

        Count++;

        if(Number == 0)
            break;
    }

    Digits = Count;

    NumBuff[Digits] = '\0';

    // Put the individual digits to Local Buffer
    while(Count)
    {
        Remainder = TempVar % 10;
        TempVar   = TempVar / 10;

        // Increment remainder by ASCII value of 0, see table
        NumBuff[Count--] = Remainder + 48;
    }

    // Put digits into UART FIFO
    for(i=0; i<=Digits; i++)
    {
        while (UART0_FR_R & UART_FR_TXFF);
        UART0_DR_R = NumBuff[i];
    }

}

// Blocking Function for getting the input as string once the buffer is not empty,
// Checks for max string size of 80 characters, Backspace, Uppercase characters and
// Terminates function when Carriage return is received.
void command_line(void)
{
    char char_input = 0;
    //int char_count =0;

    while(1)
    {
        char_input = getcUart0();

        //putcUart0(char_input);        //enable hardware echo

        if (char_input == 13)
        {
            string[char_count] = '\0';
            char_count = 0;
            break;
        }

        // Cursor processing
        if (char_input == 27)
        {
            putsUart0("esc");

            char next_1 = getcUart0();
            char next_2 = getcUart0();

            if(next_1 == 91 && next_2 == 65)            //up
            {
                putsUart0("\033[B");
                putsUart0("\033[D");
                char_input = '\0';
                continue;
            }
            else if(next_1 == 91 && next_2 == 66)       //down
            {
                putsUart0("\033[A");
                char_input = '\0';
                char_count = char_count - 1;
            }
            else if(next_1 == 91 && next_2 == 68)
            {


            }
        }

        // The infamous backspace processing
        if (char_input == 8 || char_input == 127)
        {
            if(char_count <= 0)
            {
                putsUart0("\033[C");
                continue;
            }
            else
            {
                putcUart0(' ');
                putsUart0("\033[D");
                char_count--;
                //char_count = '\0';
                continue;
            }
        }

        // Check for upper case characters
        if (char_input >= 65 && char_input <= 90)
            string[char_count++] = char_input + 32;

        else
            string[char_count++] = char_input;

        // Check for max buffer size
        if (char_count == MAX_SIZE)
        {
            putsUart0("\r\nCan't exceed more than 80 chars");    // Let the User know that character count has been exceeded
            reset_buffer();                                      // Reset the buffer, call function
            *string = 0;
            break;
        }

    }

    putsUart0("\r\n");

}


// Function for Clearing the Terminal Screen via UART
void clear_screen(void)
{
    putsUart0("\033[2J\033[H");         //ANSI VT100 escape sequence, clear screen and set cursor to home.
}


//*****************************************************************************//
//                                                                             //
//                     STRING PARSING FUNCTIONS                                //
//                                                                             //
//*****************************************************************************//

// Function to compare two strings,
// Retval: 0 if string are same, less than 0, if different
uint8_t uSTRCMP(char *string_1, char *string_2)
{
    while(*string_1 || *string_2)
    {
        if(*string_1 != *string_2)
            break;

        string_1++;
        string_2++;

    }

    return *(uint8_t*)string_1 - *(uint8_t*)string_2;


}

// Function to return the length of the string.
// retval: length of string.
uint8_t uSTRLEN(const char *string)
{
    uint8_t count = 0;

    while(*string && string++ && ++count);

    return count;

}

// Function to copy a string from source to destination
// retval: none
void uSTRCPY(char *string_dest, char *string_src)
{


    while(*string_src)
    {
        *string_dest = *string_src;

        string_dest++;
        string_src++;

    }

    *string_dest = '\0';


}


//Function for tokenizing string
void parse_string(void)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t array_shift = 0;

    //Convert character into string blocks and tokenize these blocks with delimiters
    for (i = 0; i <= uSTRLEN(string); i++)
    {
        if (string[i]== ' '|| string[i] == '\0' || string[i] == 9 || DELIMS)
        {
            new_string[args_no][args_str] = 0;
            args_no++;
            args_str = 0;
        }
        else
        {
            new_string[args_no][args_str] = string[i];
            args_str++;
        }
    }

    array_shift = 1;

    //shift the words to be printed to the starting position of the array
    while (array_shift)
    {
        array_shift = 0;

        //keep swapping elements to the right
        for (j = 0; j < args_no - 1; j++)
        {
            // Check for null elements and accordingly sort Array
            if (uSTRCMP(new_string[j], "\0") == 0 && uSTRCMP(new_string[j + 1], "\0") != 0)
            {
                array_shift = 1;

                // Exchange elements
                uSTRCPY(new_string[j], new_string[j + 1]);
                uSTRCPY(new_string[j + 1], "\0");

            }

        }
    }

    // Determine type of string for every argument
    for (j = 0; j < args_no; j++)
    {
        for (i = 0; i < uSTRLEN(new_string[j]); i++)
        {
            if (new_string[j][i] >= 97 && new_string[j][i] <= 122)          // Check if character is between a to z for the particular argument position
                a[j] = 1;                                                   // Store 1 if true at that particular argument position

            else if (new_string[j][i] >= 48 && new_string[j][i] <= 57)      // Check if character is between 0 to 9 for the particular argument position
                n[j] = 1;                                                   // Store 1 if true at that particular argument position
        }
    }

    // Update argument number from type of characters
    for (j = 0; j < args_no; j++)
    {
        if (a[j] == 0 && n[j] == 1)
        {

#ifdef DEBUG
            putsUart0("numeric string \r\n");
#endif
            args_updated++;
        }

        else if (a[j] == 1 && n[j] == 0)
        {

#ifdef DEBUG
            putsUart0("alpha string \r\n");
#endif
            args_updated++;
        }

        else if (a[j] == 1 && n[j] == 1)
        {

#ifdef DEBUG
            putsUart0("alpha numeric string \r\n");
#endif

            args_updated++;
        }
    }
}

// Function to check the argument for a particular string/verb,
// return 1, True condition
// return -1, fail
// return value: 0, default condition
int8_t is_command(char* command, uint8_t command_arg)
{
    command_arg = command_arg + 1;

    if ( (uSTRCMP(new_string[0], command) == 0) && args_updated == command_arg )
        return 1;

    else if ( (uSTRCMP(new_string[0], command) == 0) && (args_updated < command_arg || args_updated > command_arg )  )
        return -1;

    return 0;
}


//*****************************************************************************//
//                                                                             //
//                     USER TEST FUNCTIONS                                     //
//                                                                             //
//*****************************************************************************//

void test_commands(void)
{
    // LED commands Test //

    //Check arguments for string = set
    if (is_command("set", 2) == 1)
    {
        // Compare received string and then turn on Green Led
        if ( (uSTRCMP(new_string[1], "green") == 0) && (uSTRCMP(new_string[2], "on") == 0) )
        {
            putsUart0("!! Green on !! \n\r");
            ONBOARD_GREEN_LED= 1;                       // Turn Green Led on
        }

        // Compare received string and then turn on Red Led
        else if ( (uSTRCMP(new_string[1], "red") == 0) && (uSTRCMP(new_string[2], "on") == 0) )
        {
            putsUart0("!! Red on !! \n\r");
            ONBOARD_RED_LED   = 1;                       // Turn Red Led on
        }

        // Compare received string and then turn On Blue Led
        else if ( (uSTRCMP(new_string[1], "blue") == 0) && (uSTRCMP(new_string[2], "on") == 0) )
        {
            putsUart0("!! Blue on !! \n\r");
            ONBOARD_BLUE_LED  = 1;                       // Turn Blue on
        }

        // Compare received string and then turn off Red led
        else if ( (uSTRCMP(new_string[1], "red") == 0) && (uSTRCMP(new_string[2], "off") == 0) )
        {
            putsUart0("!! Red off !! \n\r");
            ONBOARD_RED_LED   = 0;                       // Turn Red Led on
        }

        // Compare received string and then turn off Blue led
        else if ( (uSTRCMP(new_string[1], "blue") == 0) && (uSTRCMP(new_string[2], "off") == 0) )
        {
            putsUart0("!! Blue off !! \n\r");
            ONBOARD_BLUE_LED   = 0;                      // Turn Blue Led on
        }

        // Compare received string and then turn off Green led
        else if ( (uSTRCMP(new_string[1], "green") == 0) && (uSTRCMP(new_string[2], "off") == 0) )
        {
            putsUart0("!! Green off !! \n\r");
            ONBOARD_GREEN_LED  = 0;                     // Turn Green Led on
        }

        else
        {
            putsUart0("Wrong arguments for \"set\" \r\n");

        }

    }
    else if (is_command("set", 2) == -1)
    {
        putsUart0("\"Set\" Command takes 2 arguments \r\n");

    }
#ifdef DEBUG
    else
    {
        putsUart0("Else condition of \"set\", command not called \r\n");

    }
#endif


    // Turn off all ports
    if (uSTRCMP(new_string[0], "off") == 0)
    {
        GPIO_PORTF_DATA_R &= ~(0xFF);                          //

    }

    // Clear the Terminal Screen
    if (uSTRCMP(new_string[0], "clear") == 0)
    {
        clear_screen();                                        // Call Clear Screen Function
        putsUart0("Screen Cleared \r\n");                      // Print to tell user that screen is cleared
    }


}

#ifdef TEST
void extern_mods_test_commands(void)
{
    // LED commands Test //

    //Check arguments for string = set
    if (is_command("external", 2) == 1)
    {
        // Compare received string and then turn on Green Led
        if ( (uSTRCMP(new_string[1], "green") == 0) && (uSTRCMP(new_string[2], "on") == 0) )
        {
            putsUart0("!! Green on !! \n\r");
            GREEN_LED= 1;                       // Turn Green Led on
        }

        // Compare received string and then turn on Red Led
        else if ( (uSTRCMP(new_string[1], "red") == 0) && (uSTRCMP(new_string[2], "on") == 0) )
        {
            putsUart0("!! Red on !! \n\r");
            RED_LED   = 1;                       // Turn Red Led on
        }

        // Compare received string and then turn On Blue Led
        else if ( (uSTRCMP(new_string[1], "blue") == 0) && (uSTRCMP(new_string[2], "on") == 0) )
        {
            putsUart0("!! Blue on !! \n\r");
            BLUE_LED  = 1;                       // Turn Blue on
        }

        // Compare received string and then turn off Red led
        else if ( (uSTRCMP(new_string[1], "red") == 0) && (uSTRCMP(new_string[2], "off") == 0) )
        {
            putsUart0("!! Red off !! \n\r");
            RED_LED   = 0;                       // Turn Red Led on
        }

        // Compare received string and then turn off Blue led
        else if ( (uSTRCMP(new_string[1], "blue") == 0) && (uSTRCMP(new_string[2], "off") == 0) )
        {
            putsUart0("!! Blue off !! \n\r");
            BLUE_LED   = 0;                      // Turn Blue Led on
        }

        // Compare received string and then turn off Green led
        else if ( (uSTRCMP(new_string[1], "green") == 0) && (uSTRCMP(new_string[2], "off") == 0) )
        {
            putsUart0("!! Green off !! \n\r");
            GREEN_LED  = 0;                     // Turn Green Led on
        }

        // Compare received string and then turn on yellow led
        else if ( (uSTRCMP(new_string[1], "yellow") == 0) && (uSTRCMP(new_string[2], "on") == 0) )
        {
            putsUart0("!! Yellow on !! \n\r");
            YELLOW_LED  = 1;                     // Turn Yellow Led on
        }

        // Compare received string and then turn on orange led
        else if ( (uSTRCMP(new_string[1], "orange") == 0) && (uSTRCMP(new_string[2], "on") == 0) )
        {
            putsUart0("!! Orange on !! \n\r");
            ORANGE_LED  = 1;                     // Turn Orange Led on
        }

        // Compare received string and then turn off yellow led
        else if ( (uSTRCMP(new_string[1], "yellow") == 0) && (uSTRCMP(new_string[2], "off") == 0) )
        {
            putsUart0("!! Yellow off !! \n\r");
            YELLOW_LED  = 0;                     // Turn Yellow Led off
        }

        // Compare received string and then turn on orange led
        else if ( (uSTRCMP(new_string[1], "orange") == 0) && (uSTRCMP(new_string[2], "off") == 0) )
        {
            putsUart0("!! Orange off !! \n\r");
            ORANGE_LED  = 0;                     // Turn Orange Led off
        }

        else
        {
            putsUart0("Wrong arguments for \"external\" \r\n");

        }

    }
    else if (is_command("external", 2) == -1)
    {
        putsUart0("\"external\" Command takes 2 arguments \r\n");

    }
#ifdef DEBUG
    else
    {
        putsUart0("Else condition of \"external\", command not called \r\n");

    }
#endif


    // Turn off all ports
    if (uSTRCMP(new_string[0], "off") == 0)
    {
        GPIO_PORTE_DATA_R &= ~(0xFF);                          //

    }


}
#endif


//*****************************************************************************//
//                                                                             //
//                  BUFFER CLEAR AND RESRET FUNCTIONS                          //
//                                                                             //
//*****************************************************************************//

// Function that resets the new_string array
void reset_new_string(void)
{
    uint8_t i = 0;
    uint8_t j = 0;

    for (i = 0; i < MAX_ARGS; i++)
    {
        for (j = 0; j < MAX_SIZE; j++)
        {
            new_string[i][j] = '\0';
        }
    }

}

// Function of reseting the input buffers and variables
void reset_buffer(void)
{
    uint8_t i = 0;

    for (i = 0; i < MAX_SIZE; i++)
    {
        string[i] = '\0';
        a[i] = '\0';
        n[i] = '\0';
        s[i] = '\0';
        buff_int[i] = '\0';
    }

    args_updated = 0;
    args_no      = 0;
    args_str     = 0;
    char_count   = 0;

    reset_new_string();

}



//*****************************************************************************//
//                                                                             //
//                     PROJECT COMMAND FUNCTIONS                               //
//                                                                             //
//*****************************************************************************//


void project_info(void)
{
    putsUart0("\033]2;| Name:Aditya Mall | (c) 2019 |\007");                                                              // Window Title Information
    putsUart0("\033]10;#FFFFFF\007");                                                                                     // Text Color (RGB)
    putsUart0("\033]11;#E14141\007");                                                                                     // Background Color (RGB)

    putsUart0("\r\n");
    putsUart0("Project: RTOS for EK-TM4C123GXL Evaluation Board.\r\n");                                                   // Project Name
    putsUart0("Name   : Aditya Mall \r\n");                                                                               // Author Name
    putsUart0("Course : EE-6314 \r\n" );                                                                                  // Author ID
    putsUart0("email  : \033[38;5;51;4maditya.mall@mavs.uta.edu\033[0m \r\n");                                            // Email Info, Foreground color:Cyan

    putsUart0("\r\n");
    putsUart0("\033[33;1m!! This Program requires Local Echo, please enable Local Echo from settings !!\033[0m \r\n");    // Foreground color:Yellow
    putsUart0("\033[33;1m!! Set Stack Size to 1024 bytes if you wish to compile and run the source code !!\033[0m \r\n");
    putsUart0("\r\n");


}


void TIVA_shell(void)
{

    //*************************************** process status command *******************************************//
    if (is_command("ps", 0) == 1)
    {
        putsUart0("Process Status \r\n");

    }
    else if (is_command("ps", 0) == -1)
    {
        putsUart0("\"ps\" command requires no arguments \r\n");
    }
#ifdef DEBUG
    else
    {
        putsUart0("Else condition of \"ps\", command not called \r\n");
    }
#endif


    //******************************************** kill command *************************************************//
    if (is_command("kill", 1) == 1)
    {
        putsUart0("kill <pid> \r\n");

    }
    else if (is_command("kill", 1) == -1)
    {
        putsUart0("\"kill\" command requires only 1 argument \r\n");
    }
#ifdef DEBUG
    else
    {
        putsUart0("Else condition of \"kill\", command not called \r\n");
    }
#endif


    //******************************************** ipcs command *************************************************//
    if (is_command("ipcs", 0) == 1)
    {
        putsUart0("Interprocess comm. \r\n");

    }
    else if (is_command("ps", 0) == -1)
    {
        putsUart0("\"ipcs\" command requires no arguments \r\n");
    }
#ifdef DEBUG
    else
    {
        putsUart0("Else condition of \"ipcs\", command not called \r\n");
    }
#endif


    //********************************************* pidof command ************************************************//
    if (is_command("pidof", 1) == 1)
    {
        putsUart0("pidof <thread name> \r\n");

    }
    else if (is_command("pidof", 1) == -1)
    {
        putsUart0("\"pidof\" command requires only 1 argument \r\n");
    }
#ifdef DEBUG
    else
    {
        putsUart0("Else condition of \"pidof\", command not called \r\n");
    }
#endif


    //********************************************* preempt command ***********************************************//
    if (is_command("preempt", 1) == 1)
    {
        putsUart0("preempt on/off \r\n");

    }
    else if (is_command("preempt", 1) == -1)
    {
        putsUart0("\"preempt\" command requires 1 argument \r\n");
    }
#ifdef DEBUG
    else
    {
        putsUart0("Else condition of \"preempt\", command not called \r\n");
    }
#endif


    //********************************************** reboot command ***********************************************//
    if (is_command("reboot", 0) == 1)
    {
        putsUart0(" \r\n");
        putsUart0("System Reboot \r\n");

        waitMicrosecond(1000000);

        NVIC_APINT_R = 0x04 | (0x05FA << 16);

    }
    else if (is_command("reboot", 0) == -1)
    {
        putsUart0("\"reboot\" command requires no arguments \r\n");
    }
#ifdef DEBUG
    else
    {
        putsUart0("Else condition of \"reboot\", command not called \r\n");
    }
#endif

    //************************************************* USER IO (echo) ********************************************//

    int i = 0;
    if ((uSTRCMP(new_string[0], "echo") == 0))
    {
        for (i = 1; i < args_updated; i++)
        {
            putsUart0(new_string[i]);
            putsUart0(" ");
        }

    }


    //************************************************ System test functions ************************************//

    if(is_command("test", 1) == 1)
    {
        if ( uSTRCMP(new_string[1], "hardware") == 0 )
        {
            putsUart0("\r\n");
            putsUart0("System Test Begin \r\n");
            testMode.external_hw_test = 1;
        }
        else if( uSTRCMP(new_string[1], "commands") == 0 )
        {
            putsUart0("\r\n");
            putsUart0("Command Test Begin, parsed strings will be displayed \r\n");
            testMode.commands = 1;
        }
        else if( uSTRCMP(new_string[1], "suspend") == 0 )
        {
            putsUart0("\r\n");
            putsUart0("Testing mode deactivated \r\n");
            testMode.commands = 0;
            testMode.external_hw_test = 0;
        }
        else
        {
            putsUart0("Wrong Argument for \"test\" \r\n");
        }

    }
    else if(is_command("test", 1) == -1)
    {
        putsUart0("ERROR:\"test\" Command takes an argument \r\n");
    }
    else
    {
        //do nothing
    }


}


//*****************************************************************************//
//                                                                             //
//                      MAIN FUNCTION ROUTINE                                  //
//                                                                             //
//*****************************************************************************//



int main(void)
{
    bool ok;

    // Initialize hardware
    initHw();

    // Project Info
    clear_screen();
    project_info();

    // Initialize OS
    rtosInit();

    // Power-up flash
    GREEN_LED = 1;
    waitMicrosecond(250000);
    GREEN_LED = 0;
    waitMicrosecond(250000);

    // Initialize semaphores
    keyPressed  = createSemaphore(1);
    keyReleased = createSemaphore(0);
    flashReq    = createSemaphore(5);
    resource    = createSemaphore(1);

    // Create Idle process
    ok  = createThread(idle, "Idle", 7);

    // Create Other Tasks
    //ok &= createThread(idle2, "Idle2", 1);
    ok &= createThread(flash4Hz,"Flash4hz", 0);
    ok &= createThread(oneshot, "OneShot", -4);


    // Start up RTOS
    if (ok)
        rtosStart(); // never returns
    else
        RED_LED = 1;

    return 0;
}