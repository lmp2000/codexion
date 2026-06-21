# Codexion Requirements Checklist

## General rules

| Requirement | Implementation idea | Test case | Status |
|---|---|---|---|
| Program name must be `codexion` | Makefile `NAME = codexion` | `make` creates `codexion` | TODO |
| Project must be written in C | `.c` and `.h` files only | Compile with cc | TODO |
| No global variables | Pass `t_sim *` everywhere | grep for global state | TODO |
| No libft | Implement only needed helpers | Check includes/libs | TODO |
| Must compile with pthread | Use `-pthread` | `make` succeeds | TODO |
| Must compile with `-Wall -Wextra -Werror` | Makefile flags | `make` no warnings | TODO |

## Arguments

| Requirement | Implementation idea | Test case | Status |
|---|---|---|---|
| Exactly 8 arguments after program name | `argc == 9` | `./codexion` fails | TODO |
| `number_of_coders` must be integer | Safe parser | `./codexion abc ...` fails | TODO |
| `time_to_burnout` must be integer | Safe parser | non-number fails | TODO |
| `time_to_compile` must be integer | Safe parser | non-number fails | TODO |
| `time_to_debug` must be integer | Safe parser | non-number fails | TODO |
| `time_to_refactor` must be integer | Safe parser | non-number fails | TODO |
| `number_of_compiles_required` must be integer | Safe parser | non-number fails | TODO |
| `dongle_cooldown` must be integer | Safe parser | non-number fails | TODO |
| Scheduler must be `fifo` or `edf` | string comparison | `random` fails | TODO |
| Negative numbers must be rejected | parser rejects `-` | `-5` fails | TODO |
| Overflow must be rejected | parse into long safely | huge number fails | TODO |

## Simulation rules

| Requirement | Implementation idea | Test case | Status |
|---|---|---|---|
| Each coder is a thread | `pthread_create` per coder | logs show coders running | TODO |
| Dongles are shared resources | `t_dongle` with mutex/condition | no duplicated dongle | TODO |
| Coder needs 2 dongles to compile | acquisition function | compile only after 2 taken logs | TODO |
| Dongle cooldown must be respected | `cooldown_until` timestamp | cooldown test | TODO |
| FIFO scheduler supported | heap by request time | FIFO ordering test | TODO |
| EDF scheduler supported | heap by deadline | EDF ordering test | TODO |
| Burnout detection required | monitor thread | burnout printed | TODO |
| Burnout must be detected quickly | monitor checks frequently | within tolerance | TODO |
| Logs must not interleave | print mutex | stress test | TODO |
| Clean shutdown required | stop flag + broadcasts | no stuck threads | TODO |
| Memory must be freed | cleanup module | Valgrind clean | TODO |