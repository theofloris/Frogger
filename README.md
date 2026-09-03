# Terminal-based Frogger Game

A Frogger clone written in C for Linux terminals, implemented in **two parallel
versions** to compare different concurrency models: a multi-process
architecture and a multi-threaded one. The two versions share the same gameplay
and visual rendering but differ in how concurrent game entities (frog,
crocodiles, plants, timer) are managed and how they communicate with the main
game loop.

The project was developed as part of the *Operating Systems* course at the
University of Cagliari.

## Features

- Classic Frogger gameplay: cross lanes of moving crocodiles, reach the lairs,
  avoid being hit by enemy bullets.
- Two crocodile types — **good** (safe to ride) and **bad** (turn red as
  warning, then dive underwater).
- Shooting mechanic: the frog can shoot bullets, and so can the enemy plants.
- Lives, score, countdown timer, win/lose menus, restart flow.
- ncurses-based rendering with custom color palette.
- Background music via SDL2_mixer.

## Project Structure

```
frogger-game/
├── frogger/         # Multi-process implementation
├── froggerThread/   # Multi-threaded implementation
├── music/           # SDL2_mixer background audio
└── README.md
```

Each subfolder is self-contained, with its own Makefile and the same module
breakdown:

| Module      | Responsibility                                              |
|-------------|-------------------------------------------------------------|
| `main`      | Entry point, game loop, spawns workers, handles game states |
| `render`    | Main rendering loop, receives updates, draws the scene      |
| `frog`      | Reads keyboard input, sends movement events                 |
| `crocodile` | Spawns and animates crocodiles in the river lanes           |
| `plants`    | Manages enemy plants on the upper bank                      |
| `bullet`    | Frog and plant projectiles                                  |
| `collision` | Frog–crocodile, frog–bullet, lair-entry detection           |
| `timer`     | Countdown timer broadcasting time updates                   |
| `menu`      | Welcome, restart, and game-over menus                       |

## Architecture

Both versions follow the same high-level design: a central **renderer** owns
the screen and the game state, while **multiple concurrent producers** (frog,
crocodiles, plants, timer) generate events that the renderer consumes and
applies. The two implementations differ only in *how* those producers are
isolated and *how* they communicate with the renderer.

### Multi-process version (`frogger/`)

Concurrency is achieved through `fork()`. The main process spawns four
long-lived child processes — `timer`, `frog`, `river`, `plants` — and the
`river` process in turn forks one child per crocodile (up to ~18 concurrent
crocodile processes), giving each one its own animation loop with independent
speed and direction.

Two communication channels carry events to the renderer:

- **Unix `pipe(2)`** — a unidirectional pipe shared by all internal game
  entities (crocodiles, plants, timer) to push their state updates to the
  renderer.
- **TCP socket on `localhost:8088`** — the frog process connects as a client
  and sends keyboard events (movement, shoot, quit). The renderer accepts the
  connection in non-blocking mode (`O_NONBLOCK` via `fcntl`), so it can
  multiplex socket reads with pipe reads inside the main loop without blocking
  on either.

PIDs of all spawned children are tracked so they can be cleanly terminated via
`SIGKILL` + `waitpid()` when the level ends, the player loses a life, or the
game is restarted.

### Multi-threaded version (`froggerThread/`)

Concurrency is achieved through POSIX `pthreads`. The main thread creates four
worker threads — `timer`, `frog`, `river`, `plants` — and the `river` thread
spawns one thread per crocodile via `pthread_create()`.

Communication uses a **shared message buffer** (a fixed-size circular array of
`msg` structs) protected by a `pthread_mutex_t`. Producers call
`write_message()` and the renderer calls `read_message()`; both functions
acquire the mutex before touching `buff_counter` and the buffer slots, so
multiple producer threads can update the game state safely. The frog still
uses the same TCP socket pattern as the process version for keyboard input,
keeping the input path consistent across the two implementations.

Threads are torn down between rounds with `pthread_cancel()` and the mutex is
re-initialized, mirroring the lifecycle that `kill()` + `fork()` provides in
the process version.

### Why two versions

Building both let me compare the trade-offs first-hand:

- **Process version**: full memory isolation (each crocodile has its own
  address space and `srand` seed), more robust against single-entity crashes,
  but heavier and requires explicit IPC (pipe + sockets) for every state
  update.
- **Thread version**: lighter weight, shared memory makes state updates
  cheaper, but every shared structure (the message buffer, the game status)
  must be protected against race conditions.

## Requirements

- Linux
- GCC
- `ncurses` (`libncurses5-dev`, `libncursesw5-dev`)
- `SDL2` and `SDL2_mixer` (`libsdl2-dev`, `libsdl2-mixer-dev`)

Install on Debian/Ubuntu:

```bash
sudo apt-get install build-essential libncurses5-dev libncursesw5-dev \
                     libsdl2-dev libsdl2-mixer-dev
```

## Build & Run

Clone and enter the repository:

```bash
git clone https://github.com/theofloris/Frogger.git
cd Frogger
```

### Multi-process version

```bash
cd frogger
make
./frogger
```

### Multi-threaded version

```bash
cd froggerThread
make
./frogger
```

To clean build artifacts:

```bash
make clean
```

> **Note:** the executable resizes the terminal to fit the playfield
> (~37×71 cells) and loads the soundtrack from `../music/song1.mp3`, so it
> should be launched from inside its own folder.

## Controls

| Key            | Action     |
|----------------|------------|
| `↑` `↓` `←` `→` | Move frog  |
| `Space`        | Shoot      |
| `q`            | Quit       |

## Credits

Developed by **Andrea Aresu** and **Theo Floris** as a university project.
Soundtrack: *CALMAN 160* (GTA IV).

## License

MIT — see [LICENSE](LICENSE).
