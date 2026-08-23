# Simpletron

SIMPLE language interpreter and LMS (Simpletron Machine Language) virtual machine.

## Build

```bash
make
```

This produces `samples/a.out`. Object files go in `build/`.

## Run

```bash
cd samples
./a.out
```

The program asks for a source file name in the current directory (for example `sum.simple`). Paths such as `samples/sum.simple` are not supported yet, so the binary lives next to the examples.

`make run` builds if needed and starts the program from `samples/`.

## SIMPLE language

Instructions: `rem`, `let`, `input`, `print`, `goto`, `if … goto`, `end`.

Variables are a single lowercase letter (`a`–`z`). Line numbers must be in increasing order.

Examples in `samples/`: `sum.simple`, `comparison.simple`, `prime.simple`.

## LMS

`src/lms.c` is the machine-code VM. It is not linked by the current `Makefile`. See `samples/sum.lms` for an example.
