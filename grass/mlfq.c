#include "process.h"
#include <string.h>
#include "queue.h"

#define USER_PID_START 5


/* [lab3-ex3]
 * initialize MLFQ data structures */
static void mlfq_init() {
    /* TODO: your code here*/

    FATAL("mlfq_init() is not implemented");
}


/* [lab3-ex3]
 * Implement the MLFQ scheduler.
 *
 * Requirements:
 *   - Return the pid of the next runnable process.
 *   - If no other process is runnable and the current process is in
 *     PROC_RUNNING, allow the current process to continue running.
 *   - If no process is runnable, return 0.
 *   - Always place system processes (pid < USER_PID_START) in the
 *     highest-priority queue.
 *   - Do not enqueue processes of state PROC_UNUSED.
 *
 * Hints:
 *   - Check the process state before selecting it to run; a dequeued
 *     process may no longer be runnable (e.g., it may be waiting).
 *   - Maintain the following invariants for a robust MLFQ implementation:
 *       -- The currently running process is not present in any queue.
 *       -- All processes except the currently running one appear in exactly one queue.
 *       -- No pid appears in more than one queue.
 *   - Use pid2idx() and idx2pid() to translate between PID and index in
 *     `proc_set` for code robustness.
 */
int mlfq() {
    /* TODO: your code here */

    FATAL("mlfq() is not implemented");
    return -1;
}


/* [lab3-ex3]
 * TODO: Update the MLFQ-related information for pid. */
void mlfq_update(int pid, enum state_transition tran, int time_units) {
    // initalize MLFQ; first called by proc_sys before schedule()
    static int initialized = 0;
    if (!initialized) {
        mlfq_init();
        initialized++;
    }

    /* TODO: your code here */



}
