# CRT Tests

## General Description of crt blkpt
```
The event count/block point is an abstraction to synchronize the
   blocking behavior of different threads on abstract events. The
   events are usually tied to a specific state of another
   data-structure (into which the blkpt is embedded).  For example, a
   lock is taken and released thus generating an event for any
   blocking threads, or a ring buffer has a data item inserted into
   it, thus generating an event for any threads waiting for
   data. Concretely, we want a number of threads to be able to block,
   and a thread to be able to wake up one, or all of them. The
   challenge is solving a single race-condition:

   thd 0: check data-structure, determine the need for blocking and
          waiting for an event
   thd 0: preemption, switching to thd 1
   thd 1: check data-structure, determine that an event is generated
   thd 1: call the scheduler, and wake all blocked threads (not
          including thd 0 yet)
   thd 1: preempt, and switch to thd 0
   thd 0: call scheduler to block

   The resulting state is that thd 1 should have unblocked thd 0, but
   due to a race, the thd 0 will be blocked awaiting the  next  event
   that may never come. Event counts are meant to solve this
   problem. Traditional systems solve this problem using condition
   variables and a lock around the scheduling logic, but if you want
   to decouple the data-structure from the scheduler (e.g. as they are
   in different modes, or components), this is a fundamental problem.

   The event count abstraction:

   Assume the data-structure generating events has at least three
   states:
   S0: available
   S1: unavailable
   S2: unavailable & subscribed

   The transitions within the data-structure are:
   {S0->S1, S1->S0, S1->S2, S2->S0}

   Every transition into S0 is an abstract  event . Threads that look
   at the state of the data-structure, and must block waiting for its
   state to change, wait for such an event to wakeup.

   The data-structure must define its own mapping to this state
   machine. A few examples:

   Mutexes:
   S0: Not locked.
   S1: Locked and held by thread 0.
   S2: Locked and held by thread 0, and threads 1...N contend the lock

   Ring buffer (for simplicity, assuming it never fills):
   S0: data items in ring buffer
   S1: no data in ring buffer
   S2: no data in ring buffer, and thread(s) are waiting for data

   The event counts are used to track the threads that use the
   data-structure when transitioning from S1->S2 (block thread), when
   it is in S2 (block additional threads), and when it transitions
   from S2->S0 (wakeup blocked threads).

   The event count is used in the following way:

   S0->S1:
       data-structure (DS) operation
       E.g. not locked -> locked, or
            dequeue from ring with single data item

   S1->S0:
       blkpt_checkpoint(ec) (not used)
       data-structure (DS) operation
       assert(blkpt_has_blocked(ec) == false) (as we're in S1)
       blkpt_trigger(ec) (won't do much as noone is blocked)
       E.g. unlock with no contention, or
            enqueue with no dequeuing threads

   S1->S2:
       cp = blkpt_checkpoint(ec)
       data-structure (DS) operation, determine we need to await event
       blkpt_wait(ec, cp)
       retry (this is why event counts can be used with lock-free data-structs)
       E.g. locked -> contended
            empty ring -> waiting for data

   S2->S0:
       data-structure (DS) operation
       assert(blkpt_has_blocked(ec) == true) (as we're in S2)
       blkpt_trigger(ec) (wake blocked threads!)
       E.g. unlock with contention, or
            enqueue with dequeuing threads

   Event count  optimization :

   We prevent the race above using an epoch (count) for the events
   thus the name. However, to avoid rapid wraparound on the epoch, we
   only increment the epoch when the race condition is possible. That
   is to say, we only increment the event count when the
   data-structure has blocked threads. This not only delays
   wraparound, it also will avoid an atomic instruction for all
   operations that don't involve blocked threads (a common-case,
   exemplified by futexes, for example).

   Usage optimization:

   Because of the event counter optimization to only use expensive
   operations when triggering there are blocked threads, the user of
   this API can trigger whenever transitioning back to S0.
  ```

## Tests to implement

- [ ] Basic Premption Check

We will have a thread preempt another thread using thd_yield().

Pseudocode
```
- set up a checkpoint on a thread
- preempt said thread using thd_yield()
- wake up all thds
- switch to original thd
- block thd (no one will wake this thd up)

```
