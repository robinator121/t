# t — template runner & quick-compiler

`t` is a small, pragmatic CLI tool that generates source files from templates (or uses existing source), compiles when needed, runs programs, and reports exit codes. Designed for Linux/UNIX environments — tiny, explicit, and useful for rapid prototyping or teaching.

## Key features
- Generate starter files from templates (per-language template folders).
- If source already exists, `t` can **auto-compile + run** sensible defaults.
- Support for C, C++, Java and Python (easy to add more).
- Per-language compile/run behavior via inheritance (compile ignored for interpreted languages).
- Specify source filename and compiled binary name (`-f`, `-o`).
- Choose a particular template (by number) or use defaults.
- Prints reliable numeric exit codes and whether process was terminated by a signal.
- Minimal dependencies: g++ (C++17), gcc, g++, javac/java, python3.

## Quick install (dev)
```bash
# compile tool (requires C++17)
g++ -std=c++17 t.cpp -o t
# move to /usr/local/bin or use locally
mv t /usr/local/bin/
