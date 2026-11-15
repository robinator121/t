# t — template runner & quick-compiler

`t` is a small, pragmatic CLI tool that generates source files from templates (or uses existing source), compiles when needed, runs programs, and reports exit codes. Designed for Linux/UNIX environments — tiny, explicit, and useful for rapid prototyping or teaching.

## Key features
- Generate starter files from templates (per-language template folders).
- If source already exists, `t` can **auto-compile + run** sensible defaults.
- Support for C, C++, Java, Python, R, D, Fortran, ASM
- Per-language compile/run behavior via inheritance (compile ignored for interpreted languages).
- Specify source filename and compiled binary name (`-f`, `-o`).
- Choose a particular template (by number) or use defaults.
- Prints reliable numeric exit codes and whether process was terminated by a signal.
- Minimal dependencies: g++ (C++17), gcc, g++, javac/java, python3.

## Quick install (dev)

The source entry point is `bin/main.cpp`, not `t.cpp`. Run these commands from the repository root on Linux:

```bash
g++ -std=c++17 bin/main.cpp -o t
sudo install -Dm755 t /usr/local/bin/t
sudo install -d /usr/share/t/templates
sudo cp -a templates/. /usr/share/t/templates/
```

The program looks for templates at `/usr/share/t/templates/<language>/template<number>`, so the template installation is required for generating new source files. Language-specific compilers and interpreters are needed for the languages you use.

An Arch Linux `PKGBUILD` is included and already builds `bin/main.cpp`. Its current package name is `t`; settle the planned Trotineta naming before publishing an AUR package.
