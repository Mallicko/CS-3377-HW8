# Assignment 8: Git and GitHub

This repository contains the files and Makefile for **Assignment 8** of CS 3377.

## Branches
- **master**: Contains the initial version of the following files:
  - `echo_s.c`
  - `echo_c.c`
  - `log_s.c`

- **second_version**: Contains the above files plus a `Makefile`.

## Files
1. **`echo_s.c`**: Server implementation.
2. **`echo_c.c`**: Client implementation.
3. **`log_s.c`**: Log server implementation.
4. **`Makefile`** (in `second_version` branch): Provides build, run, and clean targets.

## Makefile Targets
- **`make build`**: Compiles the `echo_s`, `echo_c`, and `log_s` programs.
- **`make run_echo_s`**: Runs the `echo_s` server on port 8080.
- **`make run_echo_c`**: Runs the `echo_c` client connecting to `127.0.0.1` on port 8080 using TCP.
- **`make run_log_s`**: Runs the `log_s` server.
- **`make clean`**: Removes compiled binaries.

## How to Run
1. Clone the repository:
   ```bash
   git clone git@github.com:Mallicko/CS-3377-HW8.git
   ```

2. Checkout the `second_version` branch:
   ```bash
   git checkout second_version
   ```

3. Build the project:
   ```bash
   make build
   ```

4. Run the programs using the respective targets:
   - Server:
     ```bash
     make run_echo_s
     ```
   - Client:
     ```bash
     make run_echo_c
     ```
   - Log server:
     ```bash
     make run_log_s
     ```

5. Clean up binaries:
   ```bash
   make clean
   ```
