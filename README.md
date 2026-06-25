*This project has been created as part of the 42 curriculum by lude-jes.*

# Codexion

## Description

Codexion is a C concurrency project inspired by the classic dining philosophers
problem. The simulation represents several coders sharing a limited number of USB
dongles while trying to compile, debug, and refactor without burning out.

Each coder runs in its own thread. To compile, a coder must acquire two dongles.
After compiling, the coder releases both dongles, debugs, refactors, and then tries
to compile again. The simulation stops when one coder burns out or when every
coder reaches the required number of compiles.

The project focuses on:

- POSIX threads and mutexes.
- Condition variables for thread coordination.
- A custom heap-based scheduler.
- FIFO and EDF arbitration policies.
- Deadlock and starvation prevention.
- Precise timeout monitoring.
- Serialized logging.

## Instructions

### Compilation

Build the program from the repository root:

```sh
make
```

The Makefile builds the executable:

```sh
./codexion
```

Other available rules:

```sh
make clean
make fclean
make re
```

### Execution

The program expects exactly eight mandatory arguments:

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

Arguments:

- `number_of_coders`: number of coder threads and dongles.
- `time_to_burnout`: maximum time in milliseconds before a coder burns out if they do not start compiling.
- `time_to_compile`: time in milliseconds spent compiling while holding two dongles.
- `time_to_debug`: time in milliseconds spent debugging.
- `time_to_refactor`: time in milliseconds spent refactoring.
- `number_of_compiles_required`: simulation stops once all coders have compiled at least this many times.
- `dongle_cooldown`: time in milliseconds before a released dongle can be taken again.
- `scheduler`: arbitration policy, either `fifo` or `edf`.

Invalid inputs are rejected, including missing arguments, negative numbers,
non-integers, integer overflow, zero coders, zero required compiles, or an unknown
scheduler.

Example:

```sh
make
./codexion 4 800 200 200 200 3 10 fifo
./codexion 4 800 200 200 200 3 10 edf
```

Expected log format:

```txt
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
401 1 is refactoring
```

Each line contains:

```txt
timestamp_in_ms coder_id state_message
```

## Scheduler Policies

The project implements a custom binary heap priority queue for scheduler requests.
The same heap code supports both required arbitration modes:

- `fifo`: requests are ordered by arrival timestamp.
- `edf`: requests are ordered by deadline, where the deadline is
  `last_compile_start + time_to_burnout`.

If two requests have the same priority, the lower coder id is used as a
deterministic tie-breaker.

## Blocking Cases Handled

### Deadlock prevention

The simulation avoids the classic circular wait problem by routing compile attempts
through a central scheduler. A coder must first submit a request and wait until
their request is selected before trying to take dongles.

If a coder manages to take only one dongle, the dongle is released immediately
instead of being held while waiting forever for a second one. This breaks the
hold-and-wait condition and prevents a group of coders from each keeping one
resource permanently locked.

### Coffman's conditions

The implementation addresses the relevant Coffman deadlock conditions as follows:

- Mutual exclusion exists because each dongle is protected by its own mutex.
- Hold-and-wait is reduced by releasing a partially acquired dongle when the second
  dongle is not immediately available.
- Circular wait is avoided by central scheduler arbitration before acquisition.
- No preemption is handled cooperatively: coders release dongles after compiling or
  after a failed partial acquisition.

### Starvation prevention

The scheduler prevents unchecked contention by choosing the next eligible coder
through a heap:

- FIFO serves coders in request arrival order.
- EDF prioritizes coders whose burnout deadline is closest.

This gives waiting coders a deterministic path to being selected instead of letting
threads race freely on every dongle.

### Cooldown handling

Each dongle stores a `cooldown_until` timestamp. When a dongle is released, it is
marked available but cannot be taken until the current time reaches that timestamp.
This prevents immediate reuse before the required cooldown period has elapsed.

### Precise burnout detection

A separate monitor thread checks coder deadlines independently from the coder
threads. Each coder stores the timestamp of their last compile start. If the
elapsed time exceeds `time_to_burnout`, the monitor logs the burnout event and
sets the shared stop flag.

The monitor sleeps briefly between checks so that burnout is detected quickly while
avoiding a full busy loop.

### Log serialization

All output goes through a shared logging function protected by a log mutex. This
ensures that state messages do not interleave on the same line even when multiple
threads log at the same time.

## Thread Synchronization Mechanisms

### `pthread_mutex_t`

The project uses mutexes to protect shared state:

- Each dongle has a mutex protecting its availability, owner id, and cooldown
  timestamp.
- Each coder has a mutex protecting `last_compile_time` and `compile_count`.
- The simulation state mutex protects the global stop flag.
- The scheduler mutex protects heap operations.
- The log mutex serializes terminal output.

Example race prevention:

- The monitor reads `last_compile_time` through a locked accessor while coder
  threads update it under the same mutex.
- A dongle can only be marked taken or released while its mutex is locked.
- The stop flag is read and written only through functions that lock the simulation
  state mutex.

### `pthread_cond_t`

The scheduler uses a condition variable to wake coders when the request queue
changes or when the simulation stops.

Coder flow:

1. Submit a request to the scheduler heap.
2. Wait on the scheduler condition variable.
3. Wake up and check whether their request is at the top of the heap.
4. Proceed only if selected, otherwise wait again.

When the stop flag is set, the scheduler condition variable is broadcast so that
waiting coders can exit cleanly instead of remaining blocked.

### Custom Event Behavior

The project does not use a separate event library. Event-like behavior is built
from a shared stop flag plus condition-variable broadcasts:

- The monitor sets the stop flag when burnout or completion occurs.
- Waiting coder threads are woken with `pthread_cond_broadcast`.
- Sleeping or looping coder threads periodically check the stop flag.

This provides thread-safe communication between the monitor and coder threads while
keeping the implementation within the allowed functions.

## Project Structure

```txt
include/codexion.h   Shared structures and function prototypes
src/main.c           Program entry point
src/parse.c          Argument parsing and validation
src/init.c           Simulation initialization
src/cleanup.c        Resource cleanup
src/simulation.c     Thread startup and joining
src/coder.c          Coder thread routine
src/dongle.c         Dongle acquisition, release, and cooldown
src/scheduler.c      Scheduler request flow
src/heap.c           Custom priority queue
src/monitor.c        Burnout and completion monitoring
src/state.c          Shared stop state
src/coder_state.c    Thread-safe coder state access
src/time.c           Time and sleep helpers
src/log.c            Serialized logging
```

## Resources

Classic references used for the topics covered by this project:

- `man pthread_create`
- `man pthread_join`
- `man pthread_mutex_init`
- `man pthread_mutex_lock`
- `man pthread_cond_wait`
- `man pthread_cond_broadcast`
- `man gettimeofday`
- The 42 Codexion subject PDF.
- Dining philosophers problem references for deadlock, starvation, and resource
  arbitration.
- POSIX threads documentation for mutexes, condition variables, and thread
  lifecycle management.
- Binary heap / priority queue references for scheduler implementation.

AI usage:

- AI was used to help draft and refine this README.
- AI was used to summarize the README requirements from the subject PDF.
- AI was used to describe the synchronization design in clear English.
- AI was not treated as a source of truth for correctness; the README was written
  based on the local subject PDF and the project source files.
