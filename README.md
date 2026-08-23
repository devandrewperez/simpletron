# Simpletron

SIMPLE language interpreter and LMS (Simpletron Machine Language) virtual machine.

The SIMPLE language and the LMS instruction set are described in [`docs/simpletron.md`](docs/simpletron.md).

Examples in `samples/`: `sum.simple`, `comparison.simple`, `prime.simple`. LMS sample: `sum.lms`.

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

## LMS

`src/lms.c` is the machine-code VM. It is not linked by the current `Makefile`. See [`docs/simpletron.md#lms`](docs/simpletron.md#lms) for the language reference.
