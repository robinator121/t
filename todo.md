# To Do List for t

## ✅ 1. Automatic Testing (`-b` / `--bug` flag)
- [x] Automated test runs using bug test array.
- [x] Argument handling, random input, sample files, valgrind, stress mode.
- [x] Pass/fail reporting for each test.

## 2. Template Descriptions (`-d` / `--desc` flag)
- [ ] Provide meaningful descriptions for templates (default, I/O, args, random, file ops, etc).
- [ ] CLI option to show template descriptions.

## ✅ 3. Auto Cleanup (`-c` / `--cleanup` flag)
- [x] Delete generated binaries/compiled files after execution.
- [x] Language-specific cleanup (C/C++ binaries, Java .class files).
- [x] Optional prompt before deleting.

## ✅ 4. Runner Info (`-ri` / `--rinfo` or `rv`)
- [x] Display versions of installed compilers/interpreters (java, python, gcc, g++, etc).
- [x] Useful for debugging and reproducibility.

## 5. Optional / Future Enhancements
- [ ] Random template selection if `-t` not specified.
- [ ] Inline compiler/interpreter output capture.
- [ ] Packaging for distribution.
- [ ] Test coverage reports.

## 6. Separate Modules Selection
- [ ] User-selectable modules per language (I/O, random, file ops, etc).
- [ ] Domain-specific modules (Python: AI, C: Arduino, C++: OOP demos).

## 7. Performance Measurement and Demonstrations (`demo` / `--demo`)
- [ ] Demonstration mode: time creation, compilation, run for basic templates in all languages.
- [ ] Compare timings across languages.
- [ ] Run multiple iterations for averaging.

## 8. Template Pack System & Custom Compiler Support
- [ ] Config file for template folder, custom compiler usage/version.
- [ ] Support for `.tpack` file to load templates into repository.

## 9. Educational Runtime Mode, Offline Resources
- [ ] Basic learning resources for each language (C: pointers, C++: OOP, Rust: ownership, Java: inheritance, etc).

---

**Legend:**
✅ = Complete 
[ ] = To Do
[x] = Done (subtasks) 
