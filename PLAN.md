# Codexion — Project Plan

> Goal: build Codexion step by step, with a strong understanding of the concurrency concepts before writing complex code.

Codexion is a 42 Lisboa concurrency project written in C. It is a variation of the classic Dining Philosophers problem, but with a stronger focus on resource arbitration, scheduling policies, cooldowns, precise burnout detection, and clean thread synchronization.

The project simulates coders competing for limited USB dongles. Each coder is a thread. Each dongle is a shared resource. A coder needs two dongles to compile. If a coder does not start compiling within `time_to_burnout` milliseconds since the start of the simulation or since the beginning of their last compile, they burn out.

The code itself may not be extremely large, but the concepts behind it are serious: threads, mutexes, condition variables, priority queues, fairness, starvation, deadlocks, liveness, timing precision, and safe shutdown.

---

## 0. Subject Translation

Before coding, translate the story into programming concepts.

| Subject concept | Programming concept |
|---|---|
| Coder | Thread |
| Dongle | Shared resource protected by a mutex |
| Compile | Critical section requiring two dongles |
| Debug | Timed state without dongles |
| Refactor | Timed state without dongles |
| Burnout | Death/starvation timeout |
| Scheduler | Arbitration policy for dongle access |
| FIFO | First request gets priority |
| EDF | Earliest burnout deadline gets priority |
| Monitor | Separate thread checking death/completion |

Core mental model:

```text
while simulation is running:
    acquire left and right dongles
    compile
    release dongles with cooldown
    debug
    refactor
```

The hard part is not the loop. The hard part is ensuring that all shared state is synchronized correctly and that the dongles grant access fairly according to `fifo` or `edf`.

---

## Phase 1 — Subject Decoding and Requirements Checklist

### Goal
Understand every rule of the subject and turn it into a concrete implementation checklist.

### What to study
- Program name: `codexion`.
- Language: C.
- No global variables.
- No libft.
- Required Makefile rules: `NAME`, `all`, `clean`, `fclean`, `re`.
- Compilation flags: `-Wall -Wextra -Werror -pthread`.
- Mandatory arguments:
  - `number_of_coders`
  - `time_to_burnout`
  - `time_to_compile`
  - `time_to_debug`
  - `time_to_refactor`
  - `number_of_compiles_required`
  - `dongle_cooldown`
  - `scheduler`
- Scheduler must be exactly `fifo` or `edf`.
- Reject invalid inputs:
  - negative numbers;
  - non-integers;
  - missing arguments;
  - extra arguments;
  - invalid scheduler names.

### Key decisions
- What numeric type should be used for timestamps?
- How should parsing handle overflow?
- What should the program print on invalid input?
- Should `0` be accepted for time values?
- How should `number_of_coders == 1` behave?

### Deliverable
A `REQUIREMENTS.md` or checklist inside `PLAN.md` with:

```text
Requirement | Implementation idea | Test case | Status
```

### Done when
- Every subject rule has been translated into an implementation/test item.
- Edge cases are known before coding.

---

## Phase 2 — Architecture and File Structure

### Goal
Design the project architecture before implementation.

### Suggested folder structure

```text
.
├── Makefile
├── README.md
├── PLAN.md
├── include/
│   └── codexion.h
└── src/
    ├── main.c
    ├── parse.c
    ├── init.c
    ├── cleanup.c
    ├── time.c
    ├── log.c
    ├── simulation.c
    ├── coder.c
    ├── monitor.c
    ├── dongle.c
    ├── scheduler.c
    └── heap.c
```

### File responsibilities

| File | Responsibility |
|---|---|
| `main.c` | Entry point, high-level program flow |
| `parse.c` | Argument validation and config parsing |
| `init.c` | Allocate and initialize simulation structures |
| `cleanup.c` | Free memory and destroy mutexes/condition variables |
| `time.c` | Timestamp helpers and precise sleeping |
| `log.c` | Thread-safe logging |
| `simulation.c` | Start threads, join threads, global simulation flow |
| `coder.c` | Coder thread routine |
| `monitor.c` | Burnout and completion detection |
| `dongle.c` | Dongle acquisition/release/cooldown logic |
| `scheduler.c` | FIFO/EDF comparison logic |
| `heap.c` | Custom priority queue implementation |

### Main data structures

Likely structures:

```text
t_config   -> parsed arguments
t_sim      -> shared simulation state
t_coder    -> one coder/thread state
t_dongle   -> one dongle/resource state
t_request  -> one waiting request for a dongle
t_heap     -> priority queue for dongle arbitration
```

### Important architectural rule
Because global variables are forbidden, shared state must be passed through pointers, usually from `t_sim` to coders/dongles/monitor.

### Deliverable
A final architecture sketch with each struct and file responsibility clearly defined.

### Done when
- We know where each piece of logic belongs.
- We can explain the ownership of every shared object.
- We know which mutex protects which data.

---

## Phase 3 — Parsing and Configuration

### Goal
Build a safe parser before touching threads.

### What to implement
- Exact argument count validation.
- Integer-only validation.
- Negative-number rejection.
- Overflow-safe conversion.
- Scheduler validation: only `fifo` or `edf`.
- Fill `t_config`.

### Common mistakes
- Using `atoi` blindly.
- Accepting `123abc` as valid.
- Not checking overflow.
- Accepting negative values accidentally.
- Forgetting that all arguments are mandatory.

### Test examples

```bash
./codexion
./codexion 5 800 200 200 200 7 50 fifo
./codexion 5 800 200 200 200 7 50 edf
./codexion 5 800 200 200 200 7 50 random
./codexion -5 800 200 200 200 7 50 fifo
./codexion 5 abc 200 200 200 7 50 fifo
./codexion 5 800 200 200 200 7 50 fifo extra
```

### Deliverable
Parser + config object.

### Done when
- Invalid input exits cleanly.
- Valid input fills config correctly.
- No threads are involved yet.

---

## Phase 4 — Time and Logging Foundation

### Goal
Create reliable timing and serialized logs.

### What to implement
- `get_time_ms()` using `gettimeofday()`.
- `elapsed_ms(sim)` relative to `simulation_start`.
- `precise_sleep(ms, sim)` that can stop early if simulation ends.
- Thread-safe logging with a print mutex.

### Required log formats

```text
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

### Important rule
Messages must never interleave. All output must be protected by a mutex.

### Deliverable
Timing/logging module tested with simple calls.

### Done when
- Logs are formatted correctly.
- Timestamps are relative to simulation start.
- Output is protected by a mutex.

---

## Phase 5 — Thread Skeleton

### Goal
Create the basic thread lifecycle before implementing complex dongle logic.

### What to implement
- Allocate coders.
- Create one thread per coder.
- Create one monitor thread.
- Join all threads.
- Shared stop flag protected by mutex.

### Temporary coder routine
For now, coders can simply log or sleep in a loop. The objective is only to validate thread creation and shutdown.

### Deliverable
Basic multi-threaded skeleton.

### Done when
- Threads start.
- Threads can safely read shared config.
- Threads can stop when the simulation ends.
- All threads are joined.
- No memory leaks.

---

## Phase 6 — Heap and Scheduler Logic

### Goal
Implement the custom priority queue required by the subject.

### Why this matters
The subject requires fair arbitration with a custom heap. This heap decides which coder request should be served first by each dongle.

### Scheduling policies

#### FIFO
Priority is based on request arrival time.

```text
earlier request_time = higher priority
```

#### EDF
Priority is based on earliest burnout deadline.

```text
deadline = last_compile_start + time_to_burnout
smaller deadline = higher priority
```

### Tie-breaker
EDF needs deterministic tie-breaking. Suggested order:

```text
1. earliest deadline
2. earliest request_time
3. lower coder id
```

FIFO tie-breaker:

```text
1. earliest request_time
2. lower coder id
```

### Heap operations
- insert request;
- peek best request;
- pop best request;
- compare requests by scheduler;
- handle stale/cancelled requests if needed.

### Deliverable
Standalone heap tested without threads.

### Done when
- FIFO ordering works.
- EDF ordering works.
- Tie-breakers are deterministic.
- Heap does not leak memory.

---

## Phase 7 — Dongle Model and Cooldown

### Goal
Model each dongle as a synchronized resource with cooldown and a waiting queue.

### What each dongle needs

```text
id
mutex
condition variable
availability state
owner/current holder
cooldown_until
waiting heap
```

### Dongle states
A dongle is not just locked/unlocked. It can be:

```text
available
held by a coder
released but cooling down
available but only grantable to the best queued request
```

### Cooldown rule
After a coder releases a dongle, the dongle cannot be taken again until `dongle_cooldown` milliseconds have passed.

### Deliverable
Dongle request/release logic with cooldown.

### Done when
- A dongle cannot be duplicated.
- Cooldown is respected.
- Waiting coders are woken when the dongle becomes available.

---

## Phase 8 — Safe Acquisition of Two Dongles

### Goal
Design and implement the hardest part: acquiring two dongles safely.

### Problems to prevent
- Deadlock.
- Starvation.
- Duplicated dongles.
- Broken FIFO/EDF fairness.
- Waiting forever after simulation stops.

### Classic deadlock scenario

```text
Coder 1 takes left dongle and waits for right.
Coder 2 takes left dongle and waits for right.
Coder 3 takes left dongle and waits for right.
Everyone waits forever.
```

### Possible strategies to evaluate

| Strategy | Idea | Pros | Cons |
|---|---|---|---|
| Lock ordering | Always lock lower dongle id first | Simple deadlock prevention | May complicate FIFO/EDF fairness |
| Central admission | A coordinator decides who may attempt both | Easier fairness | More shared coordination |
| Per-dongle queues | Each dongle manages its own queue | Matches subject naturally | Harder to acquire two resources atomically |
| Try-and-release | Take one, try second, release if not possible | Avoids some deadlock | Can cause livelock/starvation if not designed carefully |

### Deliverable
Final acquisition algorithm in pseudocode before C implementation.

### Done when
- We can explain why deadlock cannot happen.
- We can explain how starvation is avoided under feasible EDF conditions.
- We can explain how cooldown interacts with waiting.

---

## Phase 9 — Coder Lifecycle

### Goal
Implement the real coder routine.

### Coder loop

```text
while simulation is running:
    acquire two dongles
    log first dongle taken
    log second dongle taken
    update last_compile_start
    log compiling
    sleep time_to_compile
    increment compile_count
    release both dongles
    log debugging
    sleep time_to_debug
    log refactoring
    sleep time_to_refactor
```

### Important detail
`last_compile_start` must be updated when compiling starts, not when compiling ends.

### Completion rule
If all coders have compiled at least `number_of_compiles_required` times, the simulation stops.

### Deliverable
Full coder behavior.

### Done when
- Every compile is preceded by two dongle logs.
- Compile count is updated safely.
- Coders stop cleanly when simulation ends.

---

## Phase 10 — Monitor Thread

### Goal
Detect burnout precisely and stop the simulation.

### Monitor responsibilities
- Check every coder's deadline.
- Print burnout within 10 ms of actual burnout.
- Stop simulation on burnout.
- Stop simulation when all coders reached required compile count.
- Wake blocked coders/dongles when simulation ends.

### Burnout formula

```text
burnout_time = last_compile_start + time_to_burnout
```

If current time is greater than or equal to this value, the coder burned out.

### Precision strategy
Possible approaches:

```text
small sleep intervals, e.g. 1 ms
or pthread_cond_timedwait until the nearest deadline
```

The second approach is more elegant, but the first may be simpler if accurate enough.

### Deliverable
Monitor thread.

### Done when
- Burnout is detected within the required tolerance.
- Only one burnout message is printed.
- Simulation stops immediately after burnout.
- Success condition also stops simulation.

---

## Phase 11 — Clean Shutdown and Memory Management

### Goal
End the program safely under every scenario.

### What to handle
- Stop flag protected by mutex.
- Wake all condition variables when stopping.
- Join all coder threads.
- Join monitor thread.
- Destroy all mutexes.
- Destroy all condition variables.
- Free all allocated memory.

### Common failure case
A thread is blocked in `pthread_cond_wait()` forever because no one broadcasts after the simulation stops.

### Deliverable
Safe cleanup.

### Done when
- Program exits cleanly after burnout.
- Program exits cleanly after all coders complete required compiles.
- Valgrind shows no leaks.
- No thread remains stuck.

---

## Phase 12 — Testing Strategy

### Goal
Validate correctness incrementally.

### Test categories

| Test type | Purpose |
|---|---|
| Parser tests | Validate argument handling |
| Time tests | Validate timestamp precision |
| Logging tests | Ensure no interleaved messages |
| Heap tests | Validate FIFO/EDF ordering |
| Single coder tests | Validate edge case burnout |
| Two coder tests | Validate simple sharing |
| Many coder tests | Detect deadlocks/starvation |
| Cooldown tests | Validate dongle cooldown |
| FIFO tests | Validate arrival-order arbitration |
| EDF tests | Validate earliest-deadline arbitration |
| Burnout tests | Validate monitor precision |
| Valgrind tests | Detect leaks/invalid memory |
| Helgrind/TSAN tests | Detect data races |

### Example tests

```bash
# Valid basic run
./codexion 5 800 200 200 200 7 50 fifo

# EDF scheduling
./codexion 5 800 200 200 200 7 50 edf

# One coder should burn out
./codexion 1 800 200 200 200 3 0 fifo

# Invalid scheduler
./codexion 5 800 200 200 200 7 50 random

# Invalid negative input
./codexion -5 800 200 200 200 7 50 fifo
```

### Debugging tools

```bash
valgrind --leak-check=full --show-leak-kinds=all ./codexion ...
valgrind --tool=helgrind ./codexion ...
```

### Deliverable
A repeatable test checklist.

### Done when
- Every feature has at least one test.
- No known deadlocks.
- No known leaks.
- No obvious data races.

---

## Phase 13 — README and Defense Preparation

### Goal
Prepare the project for peer evaluation.

### README requirements
The README must be in English and include:

- First italicized line:

```text
*This project has been created as part of the 42 curriculum by <login>.*
```

- Description.
- Instructions.
- Resources.
- AI usage explanation.
- Blocking cases handled.
- Thread synchronization mechanisms.

### Must explain
- Deadlock prevention.
- Coffman's conditions.
- Starvation prevention.
- Cooldown handling.
- Precise burnout detection.
- Log serialization.
- Which mutex protects what.
- Which condition variables coordinate waiting.
- How coder threads communicate safely with the monitor.
- How the custom heap supports FIFO and EDF.

### Defense preparation questions
Be ready to answer:

```text
Why do you need mutexes?
What shared variables exist?
Which mutex protects each shared variable?
How do you prevent log interleaving?
How do you detect burnout within 10 ms?
Why does your design avoid deadlock?
How does FIFO scheduling work?
How does EDF scheduling work?
Why did you implement a heap?
How do you stop threads blocked on condition variables?
What happens with one coder?
What happens if cooldown is large?
What happens if the parameters are impossible?
```

### Deliverable
Final README + oral explanation.

### Done when
- README is complete.
- You can explain the whole architecture without reading code.
- You can modify a small part of the code during recode.

---

## Suggested Work Calendar

| Day | Focus |
|---|---|
| Day 1 | Subject decoding + requirements checklist |
| Day 2 | Architecture + structs + file responsibilities |
| Day 3 | Parser + time + logging |
| Day 4 | Thread skeleton + monitor skeleton |
| Day 5 | Heap + FIFO/EDF scheduler logic |
| Day 6 | Dongle model + cooldown |
| Day 7 | Two-dongle acquisition algorithm |
| Day 8 | Full coder lifecycle |
| Day 9 | Burnout/completion correctness |
| Day 10 | Testing + Valgrind/Helgrind + README |

---

## Implementation Order

Recommended order:

```text
1. Parse arguments safely.
2. Create config and simulation structs.
3. Implement time helpers.
4. Implement thread-safe logging.
5. Initialize coders and dongles.
6. Start simple coder threads.
7. Start monitor thread.
8. Implement heap separately.
9. Implement scheduler comparison.
10. Implement dongle queues and cooldown.
11. Implement two-dongle acquisition.
12. Implement full coder lifecycle.
13. Implement completion condition.
14. Implement clean shutdown.
15. Run tests.
16. Write README.
```

---

## Core Concepts To Master

By the end of Codexion, you should understand:

- POSIX threads;
- mutexes;
- condition variables;
- race conditions;
- deadlocks;
- starvation;
- liveness;
- fairness;
- priority queues;
- FIFO scheduling;
- EDF scheduling;
- time-based simulation;
- monitoring threads;
- clean shutdown;
- memory ownership;
- debugging concurrent programs.

---

## Personal Rule For This Project

Do not code complex synchronization logic until the algorithm can be explained in plain English.

For each important component, follow this order:

```text
1. Conceptual explanation
2. Data structure design
3. Pseudocode
4. Edge cases
5. Tests
6. C implementation
```

This project should be built through understanding, not copy-paste.
