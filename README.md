# RTOS_6314   
RTOS for coursework

## Branches
### Current Branch
* master (03/18/2019)
 
### History
* master merged with ver-1.7               (03/18/2018)
* master merged with branch_ver-1.5.5      (03/14/2019)  
* branch_ver-1.5.5 merged with ver-1.5.5.2 (03/14/2019)

* master merged with branch_ver-1.5.4      (03/11/2019) 
* master merged with branch_ver-1.5.3      (03/07/2019) 
* master merged with branch_ver-1.5.2      (02/27/2019)
* master merged with branch_ver-1.5.1      (02/25/2019)

## github repo
https://github.com/adimalla/RTOS_6314

## Change log and versions

#### Date:03/18/2019
* <b>Version 1.7</b>
* Documentation and formating changes added
* CPU task runtime calculations added
* Shell command or creating thread added as <task name> '&'

#### Date:03/14/2019
* <b>Version 1.5.5.2</b>
* 'pi' (priority inheritance) functionality added.
* ps and ipcs table formatting
* Currently testing for tasks failures

#### Date:03/13/2019
* <b>Version 1.5.5.1</b>
* 'ipcs' functionality added.
* Currently testing for tasks failures

#### Date:03/11/2019
* preempt and sched function working, (round-robin and priority scheduling selection working confirmed)
* destroythread() working and confirmed, with semaphore handling
* Shell protected from being killed

#### Date:03/06/2019
* step 13, preemptive scheduler support implemented
* step 9, modified to add round-robin and priority scheduling selection, confirmation pending 

#### Date:02/27/2019
* Lengthyfn() taking too long bug removed
* Confirmation of priorties working correctly, previously there was a problem

#### Date:02/26/2019
* Perfomance improvements in reset buffer clear loop
* Priorities added succedfully, cofirmation pending.
* All threads along with shell/cli added.

## Bugs

Date:08/17/2019
* Time constant low, reported during Project defense
* Flash4Hz solved for now, will have to do more testing.

Date:03/14/2019
* Flash4Hz crash bug root cause discovered, data overflow / error in write on 'currentPriority' struct member of tcb, which one up above the 'ticks' member.
* Attempt to Resolve in priority inheritance implemenation, priority = priority + 8;

Date:03/13/2019
* priority scheduling bug removed, solved: priority scheduling intialized to 1 in rtosInit

Date:03/12/2019
* flash4Hz bug returns, but removed with if else cases in systickISR

Date:03/09/2019
* flas4hz bug removed.

Date:03/07/2019
* flash4hz process delayed after 10 minutes, tcb[i].tick becomes max of uint32_t

Date:02/26/2019
* Cursor Bug


