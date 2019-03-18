
//******************************************************************************//
// INFO                                                                         //
//******************************************************************************//
// File            : main.c                                                     //
// Author          : Aditya Mall                                                //
// Date            : 03/14/2019                                                 //
// Copyright       : (c) 2019, Aditya Mall, Mentor: Dr. Jason Losh,             //
//                   The University of Texas at Arlington.                      //
// Project         : RTOS Framework EK-TM4C123GXL Evaluation Board.             //
// Target Platform : EK-TM4C123GXL Evaluation Board                             //
// Target uC       : TM4C123GH6PM                                               //
// IDE             : Code Composer Studio v7                                    //
// System Clock    : 40 MHz                                                     //
// UART Baudrate   : 115200                                                     //
// Data Length     : 8 Bits                                                     //
// Version         : 1.7                                                        //
// Version Control : GIT                                                        //
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
// Version 1.7 -(03/16/2019)
// info:
//      - cpu task runtime calculations added
//      - createthread shell command added
//
// Version 1.5.5.2 -(03/14/2019)
// info:
//      - 'pi' (priority inheritance) functionality added.
//      - ps and ipcs table formatting
//      - setThreadPriority function implemented and tested
//      - Currently testing for tasks failures
//
// Version 1.5.5.1 -(03/13/2019)
// info:
//      - 'ipcs' functionality added.
//      - Currently testing for tasks failures
//
// Version 1.5.5 -(03/12/2019)
// info:
//      - Flash4Hz ticks overflow bug removed with extra if else cases in systickISR
//      - 'statof' extra command added for debug purposes (not included in the coursework)
//
// Version 1.5.4 -(03/11/2019)
// info:
//      - preempt and sched function working, (round-robin and priority scheduling selection working confirmed)
//      - destroythread() working and confirmed, with semaphore handling
//      - Shell protected from being killed
//
// Version 1.5.3 -(03/06/2019)
// info:
//      - step 13, preemptive scheduler support implemented
//      - step 9, modified to add round-robin and priority scheduling selection, confirmation pending
//
// Version 1.5.2 -(02/27/2019)
// info:
//      - Lengthyfn() taking too long bug removed,
//      - Confirmation of priorities working correctly, previously there was a problem.
//
// Version 1.5.1 -(02/26/2019)
// info:
//      - step 8 and 9 added successfully, confirmation pending
//      - performance improvement in buffer clear and reset
//      - all threads added, shell thread working successfully
//
// Version 1.5-(02/23/2019)
// info:
//      - Step 4, 5, 6 and 7 added,
//      - svc offset changes #48, (queue struct member is used in processQueue array)
//
// Version 1.4-(02/09/2019)
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
#include <stdbool.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"


//*****************************************************************************//
//                                                                             //
//                  KERNEL DEFINES AND DATA STRUCTURES                         //
//                                                                             //
//*****************************************************************************//


//******************* Semaphore Defines **********************//

#define MAX_SEMAPHORES   5                             // Maximum number of semaphores
#define MAX_QUEUE_SIZE   5                             // Maximum queue size



//******************* Task Defines **********************//

// Task/Thread States
#define STATE_INVALID    0                             // No task
#define STATE_UNRUN      1                             // Task has never been run
#define STATE_READY      2                             // Has run, can resume at any time
#define STATE_DELAYED    3                             // Has run, but now awaiting timer
#define STATE_BLOCKED    4                             // Has run, but now blocked by semaphore

#define MAX_TASKS        10                            // Maximum number of valid tasks

// Processor Clocks
#define CLOCKFREQ        40000000                      // Main Clock/BUS Frequency
#define SYSTICKFREQ      1000                          // Systick timer Frequency



//******************* Kernel Data Structures **********************//

// Semaphore Data Structure
struct semaphore
{
    uint16_t count;                                    // Store Semaphore count
    uint16_t queueSize;                                // Store Queue Size
    char     semName[15];                              // Store Semaphore Name
    uint32_t processQueue[MAX_QUEUE_SIZE];             // Store PID of blocked waiting task

} semaphores[MAX_SEMAPHORES];


// Thread Control Block Data Structure
struct _tcb
{
    uint8_t  state;                                    // For Storing State Values, mentioned above in Macro defines
    void     *pid;                                     // Used to uniquely identify thread
    void     *sp;                                      // Location of stack pointer for thread
    int8_t   priority;                                 // -8 = highest to 7 = lowest
    int8_t   currentPriority;                          // Used for priority inheritance
    uint32_t ticks;                                    // Ticks until sleep complete
    char     name[16];                                 // Name of task used in ps command
    void     *semaphore;                               // Pointer to the semaphore that is blocking the thread
    uint8_t   skips;                                   // Skip count for priority calculations

} tcb[MAX_TASKS];


// Service Call ISR cases
enum svc_cases
{
    svcYIELD = 100,                                    // Value of yield label in switch case
    svcSLEEP = 101,                                    // Value of switch label in switch case
    svcWAIT  = 102,                                    // Value of wait label in switch case
    svcPOST  = 103,                                    // Value of post label in switch case
    svcKILL  = 104,                                    // Value of kill label in switch case
};

// RTOS scheduler DS
struct osScheduler
{
    uint8_t preemptiveEnable;                          // For Setting the Preemptive Scheduler Enable Mode
    uint8_t priorityEnable;                            // For Setting the Priority Scheduler Enable Mode
    uint8_t priorityInherit;                           // For Enabling Priority Inheritance
};

// DS for time calculations
struct timeCalc
{
    uint32_t runTime;                                  // Used for storing time taken by task to run
    //uint32_t totalTime;                              // Used for storing total run time of task till OS is running
    uint32_t filterTime;                               // Used for storing the moving averaged value of run time
    uint32_t taskPercentage;                           // User for storing value of CPU usage for each task

}processTime[MAX_TASKS];



//*****************************************************************************//
//                                                                             //
//                    (KERNEL) Function Prototypes                             //
//                                                                             //
//*****************************************************************************//

// Typedefs
typedef void (*_fn)();                                 // Typedef to function pointer for threads

// Kernel main Functions and Helper functions
void rtosInit();                                       // Function to Initialize RTOS.          | retval: void   | param: NULL            |
int rtosScheduler();                                   // Function to Configure RTOS Scheduler. | retval: task   | param: NULL            |
void rtosStart();                                      // Function to Start the RTOS.           | retval: void   | param: NULL            |
bool createThread(_fn fn, char name[], int priority);  // Function to Create Task/Threads.      | retval: bool   | param: Pointer To Task | param: Task Name | param: Task Priority |
void destroyThread(_fn fn);                            // Function to Destroy Task/Threads.     | retval: void   | param: Pointer To Task |
void setThreadPriority(_fn fn, uint8_t priority);      // Function to Destroy Task/Threads.     | retval: void   | param: Task Priority   |
void yield();                                          // Function to Initialize RTOS.          | retval: void   | param: NULL            |
void sleep(uint32_t tick);                             // Function to set sleep time Interval   | retval: void   | param: Value of sleep  |
uint32_t ret_R0(void);                                 // Function to get value of 1st Argument | retval: uint32 | param: NULL            |
uint32_t ret_R1(void);                                 // Function to get value of 2nd Argument | retval: uint32 | param: NULL            |
uint32_t ret_R2(void);                                 // Function to get value of 3rt Argument | retval: uint32 | param: NULL            |
uint8_t get_svcValue(void);                            // Function to get value of SVC Inst.    | retval: uint8  | param: NULL            |
void setStackPt(void* func_stack);                     // Function to set System Stack Pointer  | retval: void   | param: PID To Task     |
uint32_t* getStackPt();                                // Function to get Stack Pointer         | retval: uint32 | param: NULL            |


// Synchronization and semaphore functions
struct semaphore* \
createSemaphore(char* semName, uint8_t count);         // Function to Create Semaphores         | retval: structure pointer to sem  | param: Pointer to Sem |
void wait(struct semaphore *pSemaphore);               // Function to set Wait for a Semaphore  | retval: void                      | param: Pointer to Sem |
void post(struct semaphore *pSemaphore);               // Function to set Post for a Semaphore  | retval: void                      | param: Pointer to Sem |




//*****************************************************************************//
//                                                                             //
//                    (KERNEL) GLOBAL VARIABLES                                //
//                                                                             //
//*****************************************************************************//


// Kernel Variables
uint8_t taskCurrent = 0;                               // Index of last dispatched task
uint8_t taskCount   = 0;                               // Total number of valid tasks
uint32_t stack[MAX_TASKS][256];                        // 1024 byte stack for each thread
uint8_t semaphoreCount = 0;                            // Store Semaphore Count value
uint32_t* SystemStackPt;                               // Pointer to the Main Stack pointer

// Loop Variables
uint8_t i, j, k  = 0;                                  // Used in svcisr for loop

// SVC Variables
uint32_t* PC_VAL = 0;                                  // Used in svcisr for storing the program counter value, for inline ASM implementation of stack
uint8_t svc_value = 0;                                 // Value of service call by SVC instruction
uint32_t task_pid = 0;                                 // User for storing task PID in svcISR

// Time Calculation Variables
uint32_t startTime        = 0;                         // User for storing start time of thread
uint32_t stopTime         = 0;                         // User for storing stop time of thread
uint32_t totalTime        = 0;                         // User for storing total time taken by thread in given timeslice (80 ms in SytickISR)
uint16_t measureTimeSlice = 0;                         // User for storing value of measurement time slice in SystickISR


// Structure Variables (see structure above in Kernel defines and Data Structures )

struct _tcb tcb[MAX_TASKS] = {0};                      // Structure array for thread control block DS
struct osScheduler scheduler;                          // Structure Variable to osScheduler Structure

struct semaphore *SemaphorePt;                         // Pointer to SemaphorePt Semaphore, declared in kernel space

struct semaphore *keyPressed;                          // Pointer to keyPressed Semaphore, declared at user space
struct semaphore *keyReleased;                         // Pointer to keyReleased Semaphore, declared at user space
struct semaphore *flashReq;                            // Pointer to flashReq Semaphore, declared at user space
struct semaphore *resource;                            // Pointer to resource Semaphore, declared at user space





//*****************************************************************************//
//                                                                             //
//          (USER) MACRO DEFINITIONS, DIRECTIVES and STRUCTURES                //
//                                                                             //
//*****************************************************************************//


//*******************Debug and Code test defines**********************//
#ifndef DEBUG
//#define DEBUG
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

#define PUSH_BUTTON_0             PA2
#define PUSH_BUTTON_1             PA3
#define PUSH_BUTTON_2             PA4
#define PUSH_BUTTON_3             PA5
#define PUSH_BUTTON_4             PA6

#define BUTTON_L1                 PUSH_BUTTON_4
#define BUTTON_L2                 PUSH_BUTTON_3

#define BUTTON_R1                 PUSH_BUTTON_2
#define BUTTON_R2                 PUSH_BUTTON_1
#define BUTTON_R3                 PUSH_BUTTON_0

//************************** Project Specific Defines **********************//

#define MAX_SIZE                  40
#define MAX_ARGS                  20

// Delimiters from ASCII Table
#define DELIMS                    ( (string[i] >= 33 && string[i] <= 44) || \
                                    (string[i] >= 46 && string[i] <= 47) || \
                                    (string[i] >= 58 && string[i] <= 64) || \
                                    (string[i] >= 91 && string[i] <= 96) )


#define ARGS_CHECK(num)           (args_updated < num || args_updated > num)


// VT 100 escape sequence defines
#define __CLEAR__LINE__              (const char*)("\033[2K")
#define __RESTORE__CURSOR__POS__     (const char*)("\0338")
#define __SAVE__CURSOR__POS__        (const char*)("\0337")
#define __CURSOR__BACK__             (const char*)("\033[D")
#define __CURSOR__RIGHT__            (const char*)("\033[C")



//*****************************************************************************//
//                                                                             //
//                    (USER)Function Prototypes                                //
//                                                                             //
//*****************************************************************************//

//Delay and Blocking Functions
void waitMicrosecond(uint32_t us);                     // Microsecond Delay Function      | retval: void   | param: NULL      |


// UART IO Control functions
void putcUart0(const char c);                          // Function to Print Char through UART0   | retval: void | param: character            |
void putsUart0(const char* str);                       // Function to Print String through UART0 | retval: void | param: pointer to character |
char getcUart0(void);                                  // Function to get value through UART0    | retval: void | param: void                 |
void putnUart0(uint32_t Number);                       // Function to Print number through UART0 | retval: void | param: uint32_t             |


// VT100 Terminal Control Functions
void clear_screen(void);                               // Function which clears the terminal     | retval: void | param: void            |
void mov_right(uint16_t val);                          // Moves the Cursor Right in Terminal     | retval: void | param: Cursor position |
void set_cursor(uint32_t Line, uint32_t Cols);         // Sets Cursor position in Terminal       | retval: void | param: Line value      | param: Column Value |

// String functions
uint8_t uSTRLEN(const char *string);                   // Function to calculate string length    | retval: void | param: string      |
uint8_t uSTRCMP(char *string_1, char *string_2);       // Function to compare two strings        | retval: void | param: string 1    | param: string 2 |
void uSTRCPY(char *string_dest, char *string_src);     // Function to copy string to destination | retval: void | param: destination | param: source   |

// Shell / CLI functions
void command_line(void);                               // Command line function to take inputs from user | retval: void                 | param: void |
void parse_string(void);                               // Function to parse strings entered by user user | retval: void                 | param: void |
int8_t is_command(char* command, uint8_t arg);         // Function to enter valid verbs as commands      | retavl: -1(Fail), 1(Success) | param: verb | param: argument count |

//Project Command Functions
void project_info(void);                               // Prints the info of the project                   | retval: void | param: void      |
int8_t command_search(void);                           // Function to search for the command               | retval: void | param: void      |
void TIVA_shell(void);                                 // Shell Interpretor / CLI commands function        | retval: void | param: void      |
void getTaskPid(void);                                 // Function that prints the the task PID            | retval: void | param: void      |
void getProcessStatus(void);                           // Task manager function, displays process          | retval: void | param: void      |
void getIpcs(void);                                    // Prints the Inter process comms table             | retval: void | param: void      |
uint8_t readPbs(void);                                 // Functions for reading the testbench push buttons | retval: void | param: void      |
void getTaskStatus(char *threadName);                  // Prints the detailed status of the function       | retval: void | param: task name |

//Buffer Reset Control Functions
void reset_buffer(void);                               // Functions which resets local buffer of command line input | retval: void | param: void |
void reset_new_string(void);                           // Functions for reading the testbench push buttons          | retval: void | param: void |


//Buffer Reset Control Functions
void reset_buffer(void);                               // Functions for reseting the local string buffer for CLI | retval: void | param: void |
void reset_new_string(void);                           // Helper function called by the above function           | retval: void | param: void |


//*****************************************************************************//
//                                                                             //
//                    (USER)GLOBAL VARIABLES                                   //
//                                                                             //
//*****************************************************************************//


// String variables
char string[MAX_SIZE]               = {0};             // Array to store the chars received from UART
char new_string[MAX_ARGS][MAX_SIZE] = {0};             // Array to store the words after dividing the string to tokens
char buff_int[MAX_SIZE]             = {0};             // Array buffer to store converted value of integer to char

// Char category variables
uint8_t a[MAX_ARGS] = {0};                             // Array to store the record of Alpha characters
uint8_t n[MAX_ARGS] = {0};                             // Array to store the record of Numeric characters
uint8_t s[MAX_ARGS] = {0};                             // Array to store the record of Special characters


// Argument count variables
uint8_t args_no      = 0;                              // Variable for indexing initial number of arguments
uint8_t args_str     = 0;                              // Variable for indexing the number of characters per argument
uint8_t args_updated = 0;                              // Variable for indexing final number of arguments, not initialized to zero


// Local Directory for commands
char cmd_DB [20][15] = {"clear","sched","pidof","ps","echo","ipcs","preempt","kill","reboot", "help", "statof", "pi", "info"};

// Local Directory for Thread/Task PIDs
void *tsk_DB[MAX_TASKS] = {0};
char tskName_DB[MAX_TASKS][20] = {0};


//*****************************************************************************//
//                                                                             //
//                    (KERNEL) FUNCTIONS AND SUBROUTINES                       //
//                                                                             //
//*****************************************************************************//


void rtosInit()
{
    uint8_t i;

    // no tasks running
    taskCount = 0;

    // Default States
    scheduler.priorityEnable   = 1;                                                            // Enable Condition for Priority Scheduler
    scheduler.priorityInherit  = 1;                                                            // Enable Condition for Priority Inheritance
    scheduler.preemptiveEnable = 1;                                                            // Enable Condition for Preemption


    // clear out tcb records
    for (i = 0; i < MAX_TASKS; i++)
    {
        tcb[i].state = STATE_INVALID;
        tcb[i].pid = 0;
    }

    // REQUIRED: initialize systick for 1ms system timer
    NVIC_ST_CTRL_R     = 0;                                                                    // Clear Control bit for safe programming
    NVIC_ST_CURRENT_R  = 0;                                                                    // Start Value
    NVIC_ST_RELOAD_R   = 0x9C3F;                                                               // Set for 1Khz, (40000000/1000) - 1
    NVIC_ST_CTRL_R     = NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_ENABLE;      // set for source as clock interrupt enable and enable the timer.
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

        // Priority Scheduling
        if(scheduler.priorityEnable == 1)                                                        // Check if Priority condition is enabled (Default)
        {
           if(tcb[task].state == STATE_READY || tcb[task].state == STATE_UNRUN)                  // If yes and tasks are read and in unrun states
            {
                if(tcb[task].skips < tcb[task].currentPriority)                                  // Increment skip counts till the time it doesn't become equal to priority value
                {
                    tcb[task].skips++;                                                           // Increment skips
                    ok = false;
                }
                else if(tcb[task].skips >= tcb[task].currentPriority)                            // If greater than equal to current priority value
                {
                    tcb[task].skips = 0;                                                         // Clear Skip count
                    ok = (tcb[task].state == STATE_READY || tcb[task].state == STATE_UNRUN);     // Update value of ok to get of the loop
                }
            }
        }
        // Round-Robin Scheduling
        else                                                                                     // In Else condition it goes round-robin where everyone gets a fair chance
        {
            tcb[task].skips = 0;                                                                 // Clear Skip counts if previously coming from Priority Scheduler
            ok = (tcb[task].state == STATE_READY || tcb[task].state == STATE_UNRUN);             // Update value of ok to get of the loop
        }

    }

    return task;
}


void rtosStart()
{
    // REQUIRED: add code to call the first task to be run
    _fn fn;

    // Store Thread/Task PIDs in Local Database
    for(i = 0; i<MAX_TASKS; i++)
    {
        tsk_DB[i] = tcb[i].pid;                                                        // Store Thread PID to directory
    }

    // Store Thread/Task Name in Local Database
    for(i = 0; i<MAX_TASKS; i++)
    {
        uSTRCPY(tskName_DB[i],tcb[i].name);                                            // Store Thread Task Name to directory
    }

    // Add code to initialize the SP with tcb[task_current].sp;
    SystemStackPt  = getStackPt();                                                     // Store the first value of System/processor SP

    taskCurrent = rtosScheduler();                                                     // Run Scheduler

    setStackPt(tcb[taskCurrent].sp);                                                   // Set SP to thread SP

    fn = (_fn)tcb[taskCurrent].pid;                                                    // Store the PID current task scheduled from rtos scheduler to the function pointer

    tcb[taskCurrent].state = STATE_READY;                                              // Make Task Current state ready, Initially all are in UNRUN State, see createthread()

    (*fn)();                                                                           // Run First Task Only first Time


}

bool createThread(_fn fn, char name[], int priority)
{
    bool ok = false;
    uint8_t i = 0;
    bool found = false;

    priority = priority + 8;

    // REQUIRED: store the thread name
    // add task if room in task list
    if (taskCount < MAX_TASKS)
    {
        // make sure fn not already in list (prevent re-entrancy)
        while (!found && (i < MAX_TASKS))
        {
            found = (tcb[i++].pid == fn);
        }
        if (!found)
        {
            // find first available tcb record
            i = 0;

            while (tcb[i].state != STATE_INVALID) {i++;}
            tcb[i].state = STATE_UNRUN;                         // Set the intital state as Unrun
            tcb[i].pid = fn;                                    // Set the address of the function as PID
            tcb[i].sp = &stack[i][255];                         // Point to the user stack
            tcb[i].priority = priority;                         // Set the priority as received priority as argument
            tcb[i].currentPriority = priority;                  // Used in priority inversion
            tcb[i].skips = 0;                                   // Initial skip count is 0 for all tasks
            uSTRCPY(tcb[i].name, name);                         // Store the name of the task

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
    __asm(" SVC #104");
}

// REQUIRED: modify this function to set a thread priority
void setThreadPriority(_fn fn, uint8_t priority)
{
    uint8_t stp = 0;

    priority = priority + 8;                                      // Bias Priorities by 8, since they range from -8 to 8

    for(stp=0; stp<taskCount; stp++)
    {
        if(tcb[stp].pid == fn)                                    // Check if PID returned from argument is in TCB pid table
        {
            tcb[stp].currentPriority = priority;                  // If found then set current priority to argument priority
            break;
        }
    }

}

// Function to create semaphores
struct semaphore* createSemaphore(char* semName, uint8_t count)
{
    struct semaphore *pSemaphore = 0;

    if (semaphoreCount < MAX_SEMAPHORES)
    {
        pSemaphore = &semaphores[semaphoreCount++];              // Give the adddress of the semaphore to the semaphore pointer

        pSemaphore->count = count;                               // Set Sempahore count from the argumnets

        uSTRCPY(pSemaphore->semName, semName);                   // Record Semaphore Name

    }

    return pSemaphore;
}

// REQUIRED: modify this function to yield execution back to scheduler using pendsv
// push registers, call scheduler, pop registers, return to new function
void yield()
{
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
void systickIsr(void)
{
    uint8_t tsk = 0;                                                      // Variable for task number loop
    uint8_t firstUpdate = 1;                                              // Variable for Moving Average Filter first update


    // Sleep function support
    for(tsk=0; tsk < MAX_TASKS; tsk++)
    {
        if(tcb[tsk].state == STATE_DELAYED)                               // Check If state is DELAYED
        {

            if(tcb[tsk].ticks > 0)
                tcb[tsk].ticks--;                                         // Decrement ticks only if ticks are greater than 0

            //if(tcb[tsk].ticks == 0)                                     // If ticks are equal to zero then make state READY
            else
                tcb[tsk].state = STATE_READY;
        }
    }


    // CPU Usage measurement for different threads
    measureTimeSlice++;                                                   // Increment measurement time slice variable till it reaches 100 (100ms as systick = 1KHz)

    if(measureTimeSlice == 100)                                           // Check if timeslice is equal 100
    {

#if 0
        // Measure time when entered in systickISR if entered before task switch
        stopTime = TIMER1_TAV_R;
        TIMER1_CTL_R &= ~TIMER_CTL_TAEN;
        processTime[taskCurrent].runTime = stopTime - startTime;
#endif

        // Filter run time data (Moving Average)
        for(tsk=0; tsk<MAX_TASKS; tsk++)
        {
            if(firstUpdate)
            {
                processTime[tsk].filterTime = processTime[tsk].runTime;
                firstUpdate = 0;
            }
            else
                processTime[tsk].filterTime = processTime[tsk].filterTime * 0.9 \
                + processTime[tsk].runTime * 0.1;

        }

        totalTime = 0;

        // Calculate Total Time
        for(tsk=0; tsk<MAX_TASKS; tsk++)
        {
            totalTime = totalTime + processTime[tsk].filterTime;
            //processTime[tsk].totalTime1 = processTime[tsk].totalTime1 \
            + processTime[tsk].filterTime;

        }

        // Calculate CPU %age
        for(tsk=0; tsk<MAX_TASKS; tsk++)
        {
            processTime[tsk].taskPercentage = (processTime[tsk].filterTime * 10000) / totalTime;

            processTime[tsk].runTime = 0;
        }

        measureTimeSlice = 0;

    }

    // Preemptive scheduler support
    // Make sure there is at least one task with READY state as this is the first ISR to run,
    // in rtosInit() or will switch tasks that don't exist and will get into fault ISR.
    if(scheduler.preemptiveEnable && taskCurrent != 0)
        NVIC_INT_CTRL_R= NVIC_INT_CTRL_PEND_SV;

}


// REQUIRED: in coop and preemptive, modify this function to add support for task switching
// REQUIRED: process UNRUN and READY tasks differently
void pendSvIsr(void)
{
    __asm(" PUSH {R4-R11}");                                              // Push Register list

    tcb[taskCurrent].sp = getStackPt();                                   // save stack pointer in tcb
    setStackPt(SystemStackPt);                                            // set stack pointer to System Stack pointer


    // stop the timer
    stopTime = TIMER1_TAV_R;                                              // Save Final Value of Timer register
    TIMER1_CTL_R &= ~TIMER_CTL_TAEN;                                      // Stop Timer
    TIMER1_TAV_R = 0;                                                     // Set Timer Zero


    // Calculate time difference
    if(tcb[taskCurrent].state != STATE_INVALID)                           // Calculate for everyone but Invalid States
    {
        processTime[taskCurrent].runTime = stopTime - startTime;          // Store Time Difference
        stopTime = 0;
        startTime = 0;
    }

    // No calculations during blocked state
    if(tcb[taskCurrent].state == STATE_BLOCKED)
    {
        processTime[taskCurrent].taskPercentage = 0;                      // Clear percentage time
        processTime[taskCurrent].filterTime     = 0;                      // clear averaged time
        processTime[taskCurrent].runTime        = 0;                      // clear task run time
    }


    taskCurrent = rtosScheduler();                                        // Call RTOS Scheduler to Switch Task


    TIMER1_CTL_R |= TIMER_CTL_TAEN;                                       // Start the timer
    startTime = TIMER1_TAV_R;                                             // Save initial Value of Timer register


    if(tcb[taskCurrent].state == STATE_READY)                             // If in Ready State
    {
        setStackPt(tcb[taskCurrent].sp);                                  // Set Thread SP to Processor SP

        __asm(" POP {R4-R11}");                                           // POP Register List

    }

    else if(tcb[taskCurrent].state == STATE_UNRUN)                        // If in Unrun State, give task a new stack, as if its already is running
    {
        tcb[taskCurrent].state = STATE_READY;                             // Make State as Ready

        // In-line ASM Implementation (More Stable)
#if 1
        setStackPt(tcb[taskCurrent].sp);

        __asm(" MOV R0, #0x01000000" );                                   // 0x01000000. XPSR
        __asm(" PUSH {R0}"           );                                   // Push XPSR
        PC_VAL = tcb[taskCurrent].pid;                                    // PC value, PC at thread, PID of thread
        __asm(" PUSH {R0}"           );                                   // Push PC
        __asm(" PUSH {LR}"           );                                   // Push LR
        __asm(" PUSH {R12}"          );                                   // Push R12
        __asm(" PUSH {R0-R3}"        );                                   // Push R0 to R3

        __asm(" MOVW R8, #0xFFF9"    );                                   // Move LSB to R8 register, adjusted as per compiler pushes
        __asm(" MOVT R8, #0xFFFF"    );                                   // Move MSB to R8 register
        __asm(" PUSH {R8}"           );                                   // Push value of LR, saved in R8, (hard coded) or saved at starting of pendSvIsr
        __asm(" PUSH {R3}"           );                                   // Push compiler register

#else
        // Putting values in Thread Stack Better for Debugging

        stack[taskCurrent][255] = 0x01000000;                             // XPSR,
        stack[taskCurrent][254] = (uint32_t)tcb[taskCurrent].pid;         // PC, PID of Thread
        stack[taskCurrent][253] = 15;                                     // LR, and debug value
        stack[taskCurrent][252] = 14;                                     // R12, and debug value
        stack[taskCurrent][251] = 13;                                     // R3, and debug value
        stack[taskCurrent][250] = 12;                                     // R2, and debug value
        stack[taskCurrent][249] = 11;                                     // R1, and debug value
        stack[taskCurrent][248] = 10;                                     // R0, and debug value
        stack[taskCurrent][247] = 0xFFFFFFF9;                             // Value of LR, for compiler push
        stack[taskCurrent][246] = (uint32_t)"R3";                         // Debug Value, for compiler push

        tcb[taskCurrent].sp = &stack[taskCurrent][246];                   // Point Thread SP to stop of the thread Stack
        setStackPt(tcb[taskCurrent].sp);                                  // Set Thread SP to processor SP
#endif
        }

}


// Function for getting the value of SVC instruction
// Offset was calculated by seeing the previous value of Program counter when on,
// respective SVC instruction (SVC, #100...) using a breakpoint and then finding that,
// value, [when reached to this function (putting a break point)], in SP further offset,
// by 2 since SVC is 2 byte instruction.
uint8_t get_svcValue(void)
{
    __asm(" MOV  R0, SP"   );                                             // Move SP to R0,     (address)
    __asm(" ADD  R0, #48"  );                                             // ADD offset   ,     (address)
    __asm(" LDR  R0, [R0]" );                                             // Load offset to R0  (value)
    __asm(" SUBS R0, #2"   );                                             // Subtract 2 from R0 (value), 2 byte offset of SVC instruction
    __asm(" LDR  R0, [R0]" );                                             // Load value to R0   (value)
    __asm(" BX   LR"       );

    return 0;
}

// Empty function for returning the value of R0, 1st Argument to function
uint32_t ret_R0(void)
{
    __asm(" BX LR"          );

    return 0;
}

// Empty function for returning the value of R1 which is moved to R0, 2nd Argument to function
uint32_t ret_R1(void)
{
    __asm(" MOV R0,R1"      );
    __asm(" BX LR"          );

    return 0;
}

// Empty function for returning the value of R2 which is moved to R0, 3nd Argument to function
uint32_t ret_R2(void)
{
    __asm(" MOV R0,R2"      );
    __asm(" BX LR"          );

    return 0;
}



// REQUIRED: modify this function to add support for the service call
// REQUIRED: in preemptive code, add code to handle synchronization primitives
void svCallIsr(void)
{
    uint32_t R0 = ret_R0();                                                                         // Get value of r0
    uint32_t R1 = ret_R1();                                                                         // Get value of r1
    uint32_t R2 = ret_R2();                                                                         // Get value of r2

    svc_value = get_svcValue();                                                                     // Get SVC value

    switch(svc_value)                                                                               // Switch Case for SVC value
    {

    case svcYIELD:
                  tcb[taskCurrent].state = STATE_READY;                                             // Set task as ready

                  NVIC_INT_CTRL_R= NVIC_INT_CTRL_PEND_SV;                                           // Set pendsv bit to call pendsvISR for task switch
                  break;


    case svcSLEEP:
                  tcb[taskCurrent].ticks = R0;                                                      // Set sleep timeout value
                  tcb[taskCurrent].state = STATE_DELAYED;                                           // Set state as delayed, it can't be scheduled till the time it is not in ready state

                  NVIC_INT_CTRL_R= NVIC_INT_CTRL_PEND_SV;                                           // Set pendsv bit to call pendsvISR for task switch
                  break;

    case svcWAIT:
                  SemaphorePt = (struct semaphore*)R0;                                              // Get the pointer to the semaphore

                  if(SemaphorePt->count > 0)                                                        // Check for value of semaphore count variable
                  {
                      SemaphorePt->count--;                                                         // Decrement the count if count > 0
                      //SemaphorePt->semKey = 1;
                      tcb[taskCurrent].semaphore = NULL;
                      tcb[taskCurrent].semaphore = SemaphorePt;                                     // Must record if you are using it
                  }
                  else
                  {
                      SemaphorePt->processQueue[SemaphorePt->queueSize] =                           // Store task in semaphore process queue
                                           (uint32_t)tcb[taskCurrent].pid;

                      SemaphorePt->queueSize++;                                                     // Increment the index of the queue for next task

                      tcb[taskCurrent].state     = STATE_BLOCKED;                                   // Mark the state of of current task as blocked
                      tcb[taskCurrent].semaphore = NULL;                                            // Clear Before storing
                      tcb[taskCurrent].semaphore = SemaphorePt;                                     // Store the pointer to semaphore, record the semaphore

                      if(scheduler.priorityInherit == 1)                                            // Priority Inheritance Support
                      {
                          for(i=0; i<MAX_TASKS; i++)                                                // Find previous user of this semaphore
                          {
                              if(tcb[i].semaphore == tcb[taskCurrent].semaphore)                    // check who else uses the same semaphore, while current task is blocked
                              {
                                  if(tcb[i].currentPriority > tcb[taskCurrent].currentPriority)     // if found then check if priority value is greater than task current
                                  {
                                      tcb[i].currentPriority = tcb[taskCurrent].currentPriority;    // if yes, then give the priority value of task current(lower) to other user
                                  }
                                  break;
                              }
                          }
                      }

                      NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;                                     // Set pendsv Inside 'else' since we don't have switch task all the time
                  }
                  break;

    case svcPOST:
                 SemaphorePt = (struct semaphore*)R0;                                               // Get Pointer to the semaphore, passed as argument in post()
                 SemaphorePt->count++;                                                              // Increment the count, for other task to use resource
                 tcb[taskCurrent].currentPriority = tcb[taskCurrent].priority;                      // Restore Priority

                 if(SemaphorePt->queueSize > 0)                                                     // someone is waiting in the semaphore queue
                 {
                     for(j = 0; j < MAX_TASKS; j++)
                     {
                         if(SemaphorePt->processQueue[0] == (uint32_t)tcb[j].pid)                   // Check if a task is waiting in the same sem queue
                         {
                             SemaphorePt->processQueue[0] = 0;                                      // Release Task waiting in queue

                             tcb[j].state = STATE_READY;                                            // Make state ready of released task
                             SemaphorePt->count--;                                                  // Decrement the count, no two or more task should use the same resource

                             for(i = 0; i < SemaphorePt->queueSize; i++)
                             {
                                 SemaphorePt->processQueue[i] =
                                         SemaphorePt->processQueue[i+1];                            // Shift Semaphore process queue up

                             }

                             SemaphorePt->queueSize --;                                             // Decrement Queue Size

                             break;
                         }
                     }
                 }

                 NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;                                          // Optional, call pendSvISR to switch task
                 break;

    case svcKILL:
                 task_pid = R0;                                                                     // Value of argument from destroyThread(arg);

                 for(i=0; i<MAX_TASKS; i++)
                 {
                     if(tcb[i].pid == (_fn)task_pid)                                                // Search for requested task in list of PIDs
                     {
                         tcb[i].state = STATE_INVALID;                                              // Make the state invalid so that it cannot be scheduled
                         taskCount--;                                                               // Decrease the task count to make room for new tasks

                         if(tcb[i].semaphore != 0)                                                  // Check if task uses Semaphore
                         {
                             SemaphorePt = tcb[i].semaphore;                                        // Get the current semaphore of the task

                             for(j=0; j<SemaphorePt->queueSize; j++)
                             {
                                 if(SemaphorePt->processQueue[j] == task_pid)                       // Check if task is present in the semaphore process queue
                                 {
                                     SemaphorePt->processQueue[j] = 0;                              // Remove the task from the semaphore process queue

                                     for(k=j; k<SemaphorePt->queueSize; k++)
                                     {
                                         SemaphorePt->processQueue[k] =
                                                 SemaphorePt->processQueue[k+1];                    // Shift process queue up
                                     }

                                     SemaphorePt->queueSize --;                                     // Decrement Queue Size

                                     break;
                                 }
                                 else                                                               // Only if tasks use same semaphores in pairs
                                 {
                                     for(j=0; j<MAX_TASKS; j++)
                                     {
                                         if(SemaphorePt->processQueue[0] == (uint32_t)tcb[j].pid)   // See if someone is waiting in queue,
                                         {
                                             SemaphorePt->processQueue[0] = 0;                      // If yes, then release the task from blocked state

                                             tcb[j].state = STATE_READY;                            // Make State as ready

                                             // Shift queue up
                                             for(k=0; k<SemaphorePt->queueSize; k++)
                                             {
                                                 SemaphorePt->processQueue[k] =
                                                         SemaphorePt->processQueue[k+1];            // Shift process queue
                                             }

                                             SemaphorePt->queueSize --;                             // Decrement Queue
                                             break;
                                         }

                                     }//loop

                                     break;
                                 }// else statement end if task not found in queue

                             }// loop

                         }// check semaphore if statement

                         task_pid = 0;                                                              // Clear local task PID

                         // Clear time calculation values
                         processTime[i].runTime        = 0;                                         // Clear Run-Time value
                         processTime[i].taskPercentage = 0;                                         // Clear Task Percentage value
                         totalTime = 0;                                                             // Clear Total time for CPU% usage value

                         // Clear tcb members
                         tcb[i].pid       = NULL;                                                   // Make pid = 0 so that it can removed from found state in createthread()
                         tcb[i].sp        = NULL;                                                   // Clear Stack pointer
                         tcb[i].semaphore = NULL;                                                   // Clear Semaphore record
                         tcb[i].priority  = 0;                                                      // Clear Priority record
                         tcb[i].currentPriority  = 0;                                               // Clear Current priority Record
                         tcb[i].skips     = 0;                                                      // Clear skip counts

                         for(j = 0; j<16; j++)                                                      // Clear Name
                             tcb[i].name[j] = '\0';

                         break;
                     }// if Statement end
                 }

                 break;


    default:                                                                                        // Used for Debugging
                  putsUart0(__FUNCTION__);
                  putcUart0(':');
                  putsUart0("Entered 'Default' of switch case\r\n");
                  break;

    }

}

// Function to set SP to desired pointer value
void setStackPt(void* func_stack)
{

    __asm(" MOV SP, R0"    );
    __asm(" BX LR"         );

}

// Get address of current SP
uint32_t* getStackPt()
{

    __asm(" MOV R0,SP"     );
    __asm(" BX  LR"        );

    return 0;
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
    GPIO_PORTF_DEN_R |= (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);                  // Enable Digital
    GPIO_PORTF_DIR_R |= (1 << 1) | (1 << 2) | (1 << 3);                             // Enable as Output
    GPIO_PORTF_DIR_R &= ~(0x10);                                                    // Enable push button as Input
    GPIO_PORTF_PUR_R |= 0x10;                                                       // Enable internal pull-up for push button


    //********************************************** Console IO Hardware Configs *************************************************************//

    // Configure UART0 pins
    SYSCTL_RCGCUART_R  |= SYSCTL_RCGCUART_R0;                                       // Turn-on UART0, leave other uarts in same status
    GPIO_PORTA_DEN_R   |= 3;                                                        // Turn on Digital Operations on PA0 and PA1
    GPIO_PORTA_AFSEL_R |= 3;                                                        // Select Alternate Functionality on PA0 and PA1
    GPIO_PORTA_PCTL_R  |= GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX;                  // Select UART0 Module

    // Configure UART0 to 115200 baud, 8N1 format (must be 3 clocks from clock enable and config writes)
    UART0_CTL_R   = 0;                                                              // turn-off UART0 to allow safe programming
    UART0_CC_R   |= UART_CC_CS_SYSCLK;                                              // use system clock (40 MHz)
    UART0_IBRD_R  = 21;                                                             // r = 40 MHz / (Nx115.2kHz), set floor(r)=21, where N=16
    UART0_FBRD_R  = 45;                                                             // round(fract(r)*64)=45
    UART0_LCRH_R |= UART_LCRH_WLEN_8 | UART_LCRH_FEN;                               // configure for 8N1 w/ 16-level FIFO
    UART0_CTL_R  |= UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;                  // enable TX, RX, and module


    //***************************************************** External Modules ******************************************************************//

    // External Push Buttons
    GPIO_PORTA_DEN_R |= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6);       // Enable Digital for PORTA pins
    GPIO_PORTA_DIR_R &= ~(1 << 2) | ~(1 << 3) | ~(1 << 4) | ~(1 << 5) | ~(1 << 6);  // Clear Direction Registers, to make them Inputs
    GPIO_PORTA_PUR_R |= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6);       // Enable Internal Pull-up for push buttons

    // External LEDs
    GPIO_PORTE_DEN_R |= (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);                  // Make PORTE Pins Digital
    GPIO_PORTE_DIR_R |= (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);                  // Set direction register to make them input

    //******************************************************* Systick Timer for Measurement ****************************************************//

    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;                                      // Enable Systick timer
    TIMER1_CTL_R &= ~TIMER_CTL_TAEN;                                                // Disable timer before configuring (safe programming)
    TIMER1_CFG_R = TIMER_CFG_32_BIT_TIMER;                                          // Configure as 32-bit timer (A+B)
    TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD | TIMER_TAMR_TACDIR;                     // Configure for periodic mode and Count Up timer

}

//*****************************************************************************//
//                                                                             //
//                          DELAY FUNCTIONS                                    //
//                                                                             //
//*****************************************************************************//


// micro second delay function
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



//*****************************************************************************//
//                                                                             //
//                     UART IO Control Functions                               //
//                                                                             //
//*****************************************************************************//


// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(const char c)
{
    //while (UART0_FR_R & UART_FR_TXFF);
    UART0_DR_R = c;
    yield();
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
    while (UART0_FR_R & UART_FR_RXFE)
        yield();
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
    uint32_t d         = 0;

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

        // Increment remainder by ASCII value of 0=(48), see table
        NumBuff[Count--] = Remainder + 48;
    }

    // Put digits into UART FIFO
    for(d=0; d<=Digits; d++)
    {
        while (UART0_FR_R & UART_FR_TXFF);
        UART0_DR_R = NumBuff[d];
        yield();
    }

}

// Blocking Function for getting the input as string once the buffer is not empty,
// Checks for max string size of 80 characters, Backspace, and
// Terminates function when Carriage return is received.
void command_line(void)
{
    char char_input = 0;
    int char_count  = 0;

    putsUart0("\r\n");
    putsUart0("\033[1;32m$>\033[0m");

    while(1)
    {
        char_input = getcUart0();

        //putcUart0(char_input);                        // Enable hardware echo, Optional

        if (char_input == 13)
        {
            putsUart0("\r\n");
            string[char_count] = '\0';
            char_count = 0;
            break;
        }

        // Cursor processing
        if (char_input == 27)
        {
            char next_1 = getcUart0();
            char next_2 = getcUart0();

            if(next_1 == 91 && next_2 == 65)            // Up, don't process UP key press
            {
                putsUart0("\033[B");
                putsUart0("\033[D");
                char_input = '\0';
                continue;
            }
            else if(next_1 == 91 && next_2 == 66)       // Down, Don't process Down key press
            {
                putsUart0("\033[A");
                char_input = '\0';
                char_count = char_count - 1;
            }
            else if(next_1 == 91 && next_2 == 68)
            {


            }
        }

        // Backspace processing
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
                continue;
            }
        }

        else
            string[char_count++] = char_input;

        // Check for max buffer size
        if (char_count == MAX_SIZE)
        {
            putsUart0("\r\n");
            putsUart0("\r\nCan't exceed more than 40 characters");    // Let the User know that character count has been exceeded
            putsUart0("\r\n");

            reset_buffer();                                           // Reset the buffer, call function

            *string = 0;

            sleep(500);

            break;
        }

    }

}


// Function for Clearing the Terminal Screen via UART
void clear_screen(void)
{
    putsUart0("\033[2J\033[H");                                      // ANSI VT100 escape sequence, clear screen and set cursor to home.
}

// Function for setting the cursor terminal
void set_cursor(uint32_t Line, uint32_t Cols)
{
    putcUart0('\033');
    putcUart0('[');
    putnUart0(Line);
    putcUart0(';');
    putnUart0(Cols);
    putcUart0('H');

}

// Function for moving Cursor right in terminal
void mov_right(uint16_t val)
{
    putcUart0('\033');
    putcUart0('[');
    putnUart0(val);
    putcUart0('C');
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
        if (string[i]== ' '|| string[i] == '\0' || string[i] == 9 || string[i] == 34 )
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
//                  BUFFER CLEAR AND RESRET FUNCTIONS                          //
//                                                                             //
//*****************************************************************************//

// Function that resets the new_string array
void reset_new_string(void)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t len = 0;


    for (i = 0; i < MAX_ARGS; i++)
    {
        len = uSTRLEN(new_string[i]);

        for (j = 0; j < len; j++)
        {
            new_string[i][j] = '\0';
        }
    }

    args_updated = 0;
    args_no      = 0;
    args_str     = 0;

}

// Function of reseting the input buffers and variables
void reset_buffer(void)
{
    uint8_t i = 0;
    uint8_t len = 0;

    len = uSTRLEN(string);

    for (i = 0; i < len; i++)
    {
        string[i] = '\0';
        a[i] = '\0';
        n[i] = '\0';
        s[i] = '\0';
        buff_int[i] = '\0';
    }


    reset_new_string();

}



//*****************************************************************************//
//                                                                             //
//                     PROJECT COMMAND FUNCTIONS                               //
//                                                                             //
//*****************************************************************************//


void project_info(void)
{
    putsUart0("\033]2;| Name:Aditya Mall | (c) 2019 |\007");                                                               // Window Title Information
    putsUart0("\033]10;#FFFFFF\007");                                                                                      // Text Color (RGB)
    //putsUart0("\033]11;#E14141\007");                                                                                    // Background Color

    putsUart0("\r\n");
    putsUart0("Project : RTOS for EK-TM4C123GXL Evaluation Board.\r\n");                                                   // Project Name
    putsUart0("Name    : Aditya Mall \r\n");                                                                               // Author Name
    putsUart0("Course  : EE-6314 \r\n" );                                                                                  // Author ID
    putsUart0("email   : \033[38;5;51;4maditya.mall@mavs.uta.edu\033[0m \r\n");                                            // Email Info, Foreground color:Cyan
    putsUart0("Version : 1.7 \r\n");

    putsUart0("\r\n");
    putsUart0("\033[33;1m!! This Program requires Local Echo, please enable Local Echo from settings !!\033[0m \r\n");     // Foreground color:Yellow
    putsUart0("\r\n");

    putsUart0("\r\n");
    putsUart0("TIP:Type \"help\" to list available commands \r\n");                                                        // Display Help command message for user
    putsUart0("\r\n");

}


int8_t command_search(void)
{
    uint32_t lkp = 0;
    bool cmdFound = false;

    for(lkp=0; lkp < (sizeof(cmd_DB) / sizeof (cmd_DB[0])); lkp++)
    {
        if(uSTRCMP(cmd_DB[lkp], new_string[0])== 0 || uSTRCMP(new_string[1], "&")== 0 )
        {
            cmdFound = true;
            return 1;
        }

    }
    if(!cmdFound)
    {
        putsUart0(new_string[0]);
        putsUart0(":Command Not Found \r\n");
        putsUart0("Type \"help\" for list of Commands \r\n");
        return -1;
    }

    return 0;
}


void TIVA_shell(void)
{
    uint32_t i = 0;
    bool notFound = true;

    //************************************* Clear the Terminal Screen ******************************************//


    if (uSTRCMP(new_string[0], "clear") == 0)
    {
        clear_screen();                                        // Call Clear Screen Function
        putsUart0("Screen Cleared \r\n");                      // Print to tell user that screen is cleared
    }

    //************************************** Help (list commands) **********************************************//
    if(is_command("help", 0) == 1)
    {
        putsUart0("\r\n");
        putsUart0("List of available commands:- \r\n");                                         // Display Message for List of commands to the user
        putsUart0("\r\n");

        putsUart0("ps       : Display Process Status                                    \r\n"); // Display Process Status
        putsUart0("sched    : [rr],[priority], Select Round-Robin or Priority Scheduler \r\n"); // For selecting between round-robin scheduling or priority scheduling
        putsUart0("echo     : [args....]                                                \r\n"); // Test echo command
        putsUart0("reboot   : System Reboot                                             \r\n"); // Reboots the RTOS
        putsUart0("clear    : Clear Terminal Screen                                     \r\n"); // Clear Terminal Screen
        putsUart0("preempt  : [on/off], Preemption On/Off                               \r\n"); // Turn preemptive Scheduling On or Off
        putsUart0("pi       : [on/off], Priority Inheritance On/Off                     \r\n"); // Turn on priority Inheritance
        putsUart0("pidof    : [\"task name\"], Display PID of Task                      \r\n"); // Display PID of Task
        putsUart0("ipcs     : Inter-Process Communication Status                        \r\n"); // Display IPC status
        putsUart0("kill     : [PID], Kill Task                                          \r\n"); // Command for killing Task
        putsUart0("statof   : [\"task name\"], Display Status of Task                   \r\n"); // For Debug
        putsUart0("<Task> & : \"Task Name\" \"&\", to create new task                   \r\n"); // Command for creating a Task, should be present in directory


    }
    else if(is_command("help",0) == -1)
    {
        putsUart0("ERROR:\"help\" Command takes no argument \r\n");
    }


    //************************************* info command ******************************************//


        if (uSTRCMP(new_string[0], "info") == 0)
        {
            project_info();

        }
        else if(is_command("info",0) == -1)
        {
            putsUart0("ERROR:\"info\" Command takes no argument \r\n");
        }


    //************************************* scheduler type command ******************************************//

    if(is_command("sched", 1) == 1)
    {

        // Enable round robbin scheduler
        if(uSTRCMP(new_string[1], "rr") == 0)
        {
            scheduler.priorityEnable = 0;
            putsUart0("Round-Robin Scheduling Enabled \r\n");

        }
        // Enable default Priority Scheduler
        else if(uSTRCMP(new_string[1], "priority") == 0)
        {
            scheduler.priorityEnable = 1;
            putsUart0("Priority Scheduling Enabled \r\n");

        }
        // Check for unsupported arguments
        else
        {
            putsUart0("ERROR: unsupported argument for \"sched\"\r\n");
            putsUart0("USAGE: sched [rr/priority] \r\n");

        }

    }
    else if (is_command("sched", 1) == -1)
    {
        putsUart0("\r\n");
        putsUart0("ERROR:\"sched\" Argument Missing, USAGE: sched [rr/priority] \r\n");
        putsUart0("\r\n");
    }


    //*************************************** process status command *******************************************//
    if (is_command("ps", 0) == 1)
    {
        getProcessStatus();
    }
    else if (is_command("ps", 0) == -1)
    {
        putsUart0("ERROR:\"ps\" no arguments required \r\n");
    }



    //******************************************** kill command *************************************************//
    if (is_command("kill", 1) == 1)
    {
        uint32_t rec_pid = atoi(new_string[1]);

        // Protect Shell from being terminated by user
        for(i = 0; i<MAX_TASKS; i++)
        {
            if(tcb[i].pid == (_fn)rec_pid && tcb[i].pid > 0)
            {
                if(uSTRCMP(tcb[i].name, "Shell") == 0)
                {
                    notFound = false;
                    putsUart0("\r\n");
                    putsUart0("ERROR: Permission Denied, Shell cannot be Killed \r\n");
                    putsUart0("\r\n");
                    break;
                }
                else
                {
                    notFound = false;
                    destroyThread((_fn)rec_pid);
                    break;
                }
            }
        }
        if(notFound)
        {
            putsUart0("\r\n");
            putsUart0("ERROR:Task Not Found, USAGE: kill [PID] \r\n");
            putsUart0("Run Command \"ps\" for PID list of running Tasks \r\n");
        }

    }
    else if (is_command("kill", 1) == -1)
    {
        putsUart0("\r\n");
        putsUart0("ERROR:\"kill\" Argument Missing, USAGE: kill [PID] \r\n");
        putsUart0("\r\n");
    }



    //******************************************** ipcs command *************************************************//
    if (is_command("ipcs", 0) == 1)
    {
        getIpcs();

    }
    else if (is_command("ipcs", 0) == -1)
    {
        putsUart0("\"ipcs\" command requires no arguments \r\n");
    }



    //********************************************* pidof command ************************************************//
    if (is_command("pidof", 1) == 1)
    {
        getTaskPid();
    }
    else if (is_command("pidof", 1) == -1)
    {
        putsUart0("\r\n");
        putsUart0("ERROR:\"pidof\" Argument Missing, USAGE: pidof [\"thread name\"] \r\n");
        putsUart0("\r\n");
    }



    //********************************************* preempt command ***********************************************//
    if (is_command("preempt", 1) == 1)
    {

        // Enable Preemptive Scheduler
        if(uSTRCMP(new_string[1], "on") == 0)
        {
            scheduler.preemptiveEnable = 1;
            putsUart0("preemptive scheduler on \r\n");

        }
        // Enable default Scheduler (Cooperative), disable Preemptive
        else if(uSTRCMP(new_string[1], "off") == 0)
        {
            scheduler.preemptiveEnable = 0;
            putsUart0("preemptive scheduler off \r\n");

        }
        // Check for unsupported arguments
        else
        {
            putsUart0("ERROR: unsupported argument for \"preempt\"\r\n");
            putsUart0("USAGE: preempt [on/off] \r\n");
        }

    }
    else if (is_command("preempt", 1) == -1)
    {
        putsUart0("\r\n");
        putsUart0("ERROR:\"preempt\" Argument Missing, USAGE: preempt [on/off] \r\n");
        putsUart0("\r\n");
    }


    //********************************************* pi command ***********************************************//
    if (is_command("pi", 1) == 1)
    {

        // Enable Preemptive Scheduler
        if(uSTRCMP(new_string[1], "on") == 0)
        {
            scheduler.priorityInherit = 1;
            putsUart0("Priority Inheritance On \r\n");

        }
        // Enable default Scheduler (Cooperative), disable Preemptive
        else if(uSTRCMP(new_string[1], "off") == 0)
        {
            scheduler.priorityInherit = 0;
            putsUart0("Priority Inheritance Off \r\n");

        }
        // Check for unsupported arguments
        else
        {
            putsUart0("ERROR: unsupported argument for \"pi\"\r\n");
            putsUart0("USAGE: pi [on/off] \r\n");
        }

    }
    else if (is_command("pi", 1) == -1)
    {
        putsUart0("\r\n");
        putsUart0("ERROR:\"pi\" Argument Missing, USAGE: preempt [on/off] \r\n");
        putsUart0("\r\n");
    }



    //********************************************** reboot command ***********************************************//
    if (is_command("reboot", 0) == 1)
    {
        putsUart0(" \r\n");
        putsUart0("System Reboot \r\n");

        NVIC_APINT_R = 0x04 | (0x05FA << 16);

    }
    else if (is_command("reboot", 0) == -1)
    {
        putsUart0("\"reboot\" command requires no arguments \r\n");
    }


    //************************************************* USER IO (echo) ********************************************//

    if ((uSTRCMP(new_string[0], "echo") == 0))
    {
        for (i = 1; i < args_updated; i++)
        {
            putsUart0(new_string[i]);
            putsUart0(" ");
        }

    }


    //********************************************* statof command ************************************************//

    if (is_command("statof", 1) == 1)
    {
        getTaskStatus(new_string[1]);
    }
    else if (is_command("statof", 1) == -1)
    {
        putsUart0("\r\n");
        putsUart0("ERROR:\"statof\" Argument Missing, USAGE: statof [\"thread name\"] \r\n");
        putsUart0("\r\n");
    }

    //********************************************* createthread command  ************************************************//

    if (uSTRCMP(new_string[1], "&") == 0)
    {

        bool fnd = false;

        // Check in thread DS
        for(i=0; i<MAX_TASKS; i++)
        {
            if(uSTRCMP(new_string[0], tcb[i].name) == 0)
            {

                fnd = true;
                putsUart0("Task already lives can't create \r\n");
                break;

            }
        }

        // if not found check in local DB
        if(!fnd)
        {
            for(i=0; i<MAX_TASKS; i++)
            {
                if(uSTRCMP(new_string[0], tskName_DB[i]) == 0)
                {
                    fnd = true;
                    putsUart0("Task already killed before, lets create \r\n");
                    createThread((_fn)tsk_DB[i], tskName_DB[i], 0);
                    break;
                }
            }
        }

        // Still not found
        if(fnd == false)
            putsUart0("Task Not Found \r\n");


    }


    //************************************************** TivaShell Function End ************************************************************************//
}



// Function to get the Task PID
void getTaskPid(void)
{
    uint8_t taskNo = 0;

    for(taskNo=0; taskNo<MAX_TASKS; taskNo++)
    {
        if(uSTRCMP(new_string[1], tskName_DB[taskNo]) == 0)
        {
            putsUart0("PID:");
            putnUart0((uint32_t)tcb[taskNo].pid);
            putsUart0("\r\n");
        }
    }

}


// Function get Process Status
void getProcessStatus(void)
{
    uint8_t taskNo = 0;
    uint8_t len_diff = 0;
    char int_buf[3] = {0};
    char stateName[10] = {0};

    uint8_t cpuNUM = 0;
    uint32_t cpuP = 0;


    putsUart0("\r\n");

    //Scheduler Status
    putsUart0("\033[33;1m"); putsUart0("Scheduler Status"); putsUart0("\r\n"); putsUart0("\033[0m");

    //Priority Status
    putsUart0("Priority:");
    if(scheduler.priorityEnable == 1)
    {
        putsUart0("\033[32;1m"); putsUart0("ON"); putsUart0("\033[0m");
    }
    else
    {
        putsUart0("\033[31;1m"); putsUart0("OFF"); putsUart0("\033[0m");
    }

    //Preemptive Status
    mov_right(4);
    putsUart0("Preemption:");
    if(scheduler.preemptiveEnable == 1)
    {
        putsUart0("\033[32;1m"); putsUart0("ON"); putsUart0("\033[0m");
    }
    else
    {
        putsUart0("\033[31;1m"); putsUart0("OFF"); putsUart0("\033[0m");
    }

    //Inheritance Status
    mov_right(3);
    putsUart0("Priority Inheritance:");
    if(scheduler.priorityInherit == 1)
    {
        putsUart0("\033[32;1m"); putsUart0("ON"); putsUart0("\033[0m");
    }
    else
    {
        putsUart0("\033[31;1m"); putsUart0("OFF"); putsUart0("\033[0m");
    }

    putsUart0("\r\n");

    putsUart0("\r\n");

    // Title
    putsUart0("\033[33;1m");
    putsUart0("PID"); mov_right(5);putsUart0("Task Name"); mov_right(5);putsUart0("CPU%"); mov_right(4);putsUart0("Priority");
    mov_right(3);putsUart0("STATE"); putsUart0("\r\n"); putsUart0("\033[0m");

    for(taskNo=0; taskNo < MAX_TASKS; taskNo++)
    {
        cpuP = processTime[taskNo].taskPercentage;

        if(tcb[taskNo].pid || !(tcb[taskNo].state == STATE_INVALID))                                    // Don't show task with INVALID Status
        {
            if((uint32_t)tcb[taskNo].pid < 10000)
            {
                putnUart0(0);
                putnUart0((uint32_t)tcb[taskNo].pid);
            }
            else
                putnUart0((uint32_t)tcb[taskNo].pid);

            putsUart0("   ");

            putsUart0(tcb[taskNo].name);

            len_diff = abs(uSTRLEN(tcb[taskNo].name) - uSTRLEN("Task Name"));


            // Print CPU task Run Times
            mov_right(len_diff+4);

            cpuNUM = cpuP / 100;

            if(cpuNUM < 10)
                putsUart0("0");

            putnUart0(cpuNUM);
            cpuNUM = 0;

            putcUart0('.');

            cpuNUM = cpuP % 100;

            putnUart0(cpuNUM);

            if(cpuNUM < 10)
                putsUart0("0");

            cpuNUM = 0;


            // Print Thread priorities
            mov_right(7);

            if(tcb[taskNo].currentPriority < 8)
            {
                ltoa((long)tcb[taskNo].currentPriority - 8,int_buf);
                putsUart0(int_buf);
            }
            else
            {
                putnUart0(0);
                putnUart0(tcb[taskNo].currentPriority - 8);
            }

            // Parse state names
            switch(tcb[taskNo].state)
            {
            case 0:
                uSTRCPY(stateName, "INVALID");
                break;

            case 1:
                uSTRCPY(stateName, "UNRUN");
                break;

            case 2:
                uSTRCPY(stateName, "READY");
                break;

            case 3:
                uSTRCPY(stateName, "DELAYED");
                break;

            case 4:
                uSTRCPY(stateName, "BLOCKED");
                break;

            default:
                break;
            }

            mov_right(6);

            putsUart0(stateName);

            putsUart0("\r\n");
        }
    }

    putsUart0("\r\n");
}


// Function to get Inter-Process Communication Status, currently displays only semaphore status
void getIpcs(void)
{
    uint8_t semNo     = 0;                     // Variable used in Semaphore loop
    uint8_t len_diff  = 0;                     // Column width calculations
    uint8_t col_width = 0;                     // store value of table column width
    uint8_t taskNo    = 0;
    uint8_t q = 0;
    uint8_t name_len[MAX_SEMAPHORES] = {0};


    putsUart0("\r\n");

    // Title
    putsUart0("\033[33;1m");
    putsUart0("Sem Name"); mov_right(5);putsUart0("Sem Count"); mov_right(3);putsUart0("Queue Size"); mov_right(3);putsUart0("Running");
    mov_right(4);putsUart0("Waiting"); putsUart0("\r\n"); putsUart0("\033[0m");


    for(semNo=0; semNo < semaphoreCount; semNo++)                                                              // Find Max Column Width
        name_len[semNo] = uSTRLEN(semaphores[semNo].semName);                                                  // Max length of name = Max Column width

    col_width = name_len[0];                                                                                   // First Value = first element of Array

    for(semNo=0; semNo < semaphoreCount; semNo++)                                                              // Calculate Max Column Width
    {
        if(name_len[semNo] > col_width)                                                                        // Highest Number in Array
            col_width = name_len[semNo];
    }

    // Print Data
    for(semNo=0; semNo < semaphoreCount; semNo++)
    {
        putsUart0(semaphores[semNo].semName);                                                                  // Print Semaphore Name
        len_diff = 0;
        len_diff = abs(uSTRLEN(semaphores[semNo].semName) - col_width);

        mov_right(5 + len_diff);
        if(semaphores[semNo].count < 10)                                                                       // Print Semaphore Count
        {
            putnUart0(0);
            putnUart0(semaphores[semNo].count);                                                                //preceded by 0 if smaller than 10
        }
        else
        {
            putnUart0(semaphores[semNo].count);
        }

        mov_right(11);
        putnUart0(0);                                                                                         // Print Queue Size
        putnUart0(semaphores[semNo].queueSize);

        mov_right(7);
        for(taskNo=0; taskNo<MAX_TASKS; taskNo++)                                                             // Print Tasks that are running
        {
            if(&semaphores[semNo] == tcb[taskNo].semaphore)
            {
                if((uint32_t)tcb[taskNo].state != STATE_BLOCKED)
                {
                    putsUart0(tcb[taskNo].name);
                    break;
                }
                else
                {
                    for(len_diff = 0; len_diff < uSTRLEN("Waiting  "); len_diff++)                            // Calculate Column width, waiting__ taken as
                        putcUart0(' ');

                    break;
                }
            }
        }

        mov_right(1);                                                                                         // Move cursor by 1 space
        for(taskNo=0; taskNo<MAX_TASKS; taskNo++)                                                             // Print Tasks that are waiting
        {
            if(&semaphores[semNo] == tcb[taskNo].semaphore)                                                   // check which task uses that semaphore
            {

                for(q=0; q<semaphores[semNo].queueSize; q++)                                                  // Check in process queue
                {
                    if(semaphores[semNo].processQueue[q] == (uint32_t)tcb[taskNo].pid)                        // if task is found in queue
                    {
                            putsUart0(" ");
                            putsUart0(tcb[taskNo].name);                                                      // Print Task

                    }
                }

#if 0
                // For Extra fields
                for(len_diff = 0; len_diff < uSTRLEN("Waiting  "); len_diff++)
                    putcUart0(' ');
#endif
            }
        }

        putsUart0("\r\n");
    }

    putsUart0("\r\n");
}


// Debug Function to get detailed status of a Task (Not in Course Work)
void getTaskStatus(char *threadName)
{
    uint8_t taskNo  = 0;
    uint8_t taskFnd = 0;

    for(taskNo=0; taskNo<MAX_TASKS; taskNo++)
    {
        if(uSTRCMP(threadName,tcb[taskNo].name) == 0)              // Check of name exists in table
        {
            // Print Thread Name
            putsUart0("\r\n");
            putsUart0("Task         : ");
            putsUart0(threadName);
            putsUart0("\r\n");

            // Print Sleep ticks
            putsUart0("Sleep Ticks  : ");
            putnUart0(tcb[taskNo].ticks);
            putsUart0("\r\n");

            // Print Skip Count
            putsUart0("Skips        : ");
            putnUart0(tcb[taskNo].currentPriority);
            putsUart0("\r\n");

            // Print Current Skips
            putsUart0("Current Skips: ");
            putnUart0(tcb[taskNo].skips);
            putsUart0("\r\n");

            taskFnd = 1;

            break;
        }
    }

    if(taskFnd == 0)                                              // If not found then print not found
        putsUart0("ERROR:Task Not Found \r\n");

}


// Function to get a return value from buttons pushed in OS test bench Hardware.
uint8_t readPbs(void)
{
    uint8_t retval_button = 0;

    if(!PUSH_BUTTON_0)
        retval_button |= 1;

    if(!PUSH_BUTTON_1)
        retval_button |= 2;

    if(!PUSH_BUTTON_2)
        retval_button |= 4;

    if(!PUSH_BUTTON_3)
        retval_button |= 8;

    if(!PUSH_BUTTON_4)
        retval_button |= 16;

    return retval_button;
}



//*****************************************************************************//
//                                                                             //
//                        USER THREADS / TASKS                                 //
//                                                                             //
//*****************************************************************************//

// one task must be ready at all times or the scheduler will fail
// the idle task is implemented for this purpose


void idle()
{
    while(true)
    {
        ORANGE_LED = 1;
        waitMicrosecond(1000);
        ORANGE_LED = 0;
        yield();
    }
}


void flash4Hz()
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


void partOfLengthyFn()
{
    // represent some lengthy operation
    waitMicrosecond(990);
    // give another process a chance to run
    yield();
}



void lengthyFn()
{
    uint16_t i;
    while(true)
    {
        wait(resource);
        for (i = 0; i < 5000; i++)
        {
            partOfLengthyFn();
        }
        RED_LED ^= 1;
        post(resource);
    }
}



void readKeys()
{
    uint8_t buttons;
    while(true)
    {
        wait(keyReleased);
        buttons = 0;
        while (buttons == 0)
        {
            buttons = readPbs();
            yield();
        }
        post(keyPressed);
        if ((buttons & 1) != 0)
        {
            YELLOW_LED ^= 1;
            RED_LED = 1;
        }
        if ((buttons & 2) != 0)
        {
            post(flashReq);
            RED_LED = 0;
        }
        if ((buttons & 4) != 0)
        {
            createThread(flash4Hz, "Flash4Hz", 0);
        }
        if ((buttons & 8) != 0)
        {
            destroyThread(flash4Hz);
        }
        if ((buttons & 16) != 0)
        {
            setThreadPriority(lengthyFn, 4);
        }
        yield();
    }
}


void debounce()
{
    uint8_t count;
    while(true)
    {
        wait(keyPressed);
        count = 10;
        while (count != 0)
        {
            sleep(10);
            if (readPbs() == 0)
                count--;
            else
                count = 10;
        }
        post(keyReleased);
    }
}


void uncooperative()
{
    while(true)
    {
        while (readPbs() == 8)
        {
        }
        yield();
    }
}


void important()
{
    while(true)
    {
        wait(resource);
        BLUE_LED = 1;
        sleep(1000);
        BLUE_LED = 0;
        post(resource);
    }
}


void shell()
{
    clear_screen();                      // Clear Screen

    project_info();                      // Display Project Info

    while(true)
    {
        command_line();                  // Call the command line function for user imput

        parse_string();                  // Parse user input

        if(command_search() == 1)        // Search for commands entered by user
            TIVA_shell();                // Execute Commands if found

        reset_buffer();                  // Clear Buffer before exit

        yield();
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

    // Initialize OS
    rtosInit();

    // Power-up flash
    GREEN_LED = 1;
    waitMicrosecond(250000);
    GREEN_LED = 0;
    waitMicrosecond(250000);

    // Initialize semaphores
    keyPressed  = createSemaphore("keyPressed", 1);
    keyReleased = createSemaphore("keyReleased", 0);
    flashReq    = createSemaphore("flashReq", 5);
    resource    = createSemaphore("resource",1);

    // Create Idle process
    ok  = createThread(idle, "Idle", 7);

    // Create Other Tasks
    ok &= createThread(lengthyFn, "LengthyFn", 4);
    ok &= createThread(flash4Hz,"Flash4Hz", 0);
    ok &= createThread(oneshot, "OneShot", -4);
    ok &= createThread(readKeys, "ReadKeys", 4);
    ok &= createThread(debounce, "Debounce", 4);
    ok &= createThread(important, "Important", -8);
    ok &= createThread(uncooperative, "Uncoop", 2);
    ok &= createThread(shell, "Shell", 0);


    // Start up RTOS
    if (ok)
        rtosStart(); // never returns
    else
        RED_LED = 1;

    return 0;
}
