# To Do List for t

## 1. Automatic Testing (-b / --bug flag)
- Implement automated test runs using an array of bug tests.
Examples:
- Test argument handling: "./a.out asd asd ads adsa"
- Random inputs from /dev/urandom
- Sample input files for the program to process
- Ensure proper reporting of pass/fail for each test.
- Valgrind mode
- Stress mode (nuking stdin with random inputs)

## 2. Template Descriptions (-d / --desc flag)
- Provide meaningful descriptions for templates:
0 - Default template
1 - Basic I/O (keyboard input/output)
2 - Command line arguments
3 - Random number generation
4 - File operations
5 - [Add more as needed]
- Allow the CLI to show template descriptions.

## 3. Auto Cleanup (-c / --cleanup flag)
- Delete generated binaries or compiled files after execution:
- C/C++: ./a.out (or custom output)
- Java: .class files
- Optional: configurable cleanup (prompt before deleting).

## 4. Runner Info (-ri / --rinfo)
- Display versions of installed compilers/interpreters:
- java --version
- python --version
- gcc --version
- g++ --version
- Useful for debugging environment issues and ensuring reproducibility.

## 5. Optional / Future Enhancements
- Random template selection if -t not specified.
- Inline compiler/interpreter output capture (instead of relying on system console).
- Packaging (mkpkg or PKGBUILD) for distribution.
- Test coverage reports for auto-testing.

## 6. Seperate modules selection
- Enable the user to choose which modules (for example, random numbers, file I/O, etc...) to use and incorporate into the language.
- Programming language domain specific modules, for example:
    - For Python, Pytorch, AI and big data modules to load as classes
    - For C, Arduino, kernel module, low resource environment modules
    - For C++, demonstrations of no garbage collection but OOP features
    - etc...
    
## 7. Performance measurement and demonstrations
- Incorporate a demonstration mode, where the program would time creation, compilation, running basic templates in all languages, comparing them to each other.
- Time individual codes, outputting time differences between iterations (it would run multiple times to avoid external factors)

## 8. Template pack system and custom compiler support
- Config file where the user can specify template folder location, custom compiler versions/usage (ex: Java 25 instead of Java 21)
- Adding the ability to create a tpack file, which contains template files that t automatically loads into it's template file repository.

## 9. Educational runtime mode, offline resources
- Incorporate basic FOSS compliant learning resources, introducing the users to concepts within each programming language. Examples would include:
    - C: Basics, Pointers, Memory management, Strings, I/O
    - C++: Classes, Polymorphism, Namespaces
    - Rust: Ownership, Traits, Modules
    - Java: Classes, Methods, References, Inheritance
    - etc...
