#include <iostream>
#include <cstring>
#include <fstream>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/wait.h>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
using namespace std;


string to_upper(const string &s)
{
    string out;
    for (char c : s) out.push_back((char) toupper((unsigned char)c));
    return out;
}

string strip_extension(const string &fname)
{
    size_t pos = fname.rfind('.');
    if (pos == string::npos) return fname;
    return fname.substr(0, pos);
}

int decode_system_status(int status, const string &label)
{
    if (status == -1) {
        cout << label << " : system() failed to start command\n";
        return -1;
    }
    if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        cout << label << " exit code: " << code << "\n";
        return code;
    } else if (WIFSIGNALED(status)) {
        int sig = WTERMSIG(status);
        cout << label << " terminated by signal: " << sig << "\n";
        return 128 + sig; // common convention for some reason
    } else {
        cout << label << " finished with status: " << status << "\n";
        return status;
    }
}


class language
{
protected:
    bool r;
    bool comp;
    short tn;
    string filename;
    string langname; // e.g. "java", "c", "cpp", "python"

    language(bool r, bool comp, short tn, string filename, string lang)
    {
        this->comp = comp;
        if (r == 1)
        {
            this->r = true;
            this->comp = true;
        }
        else {
            this->r = r;
        }
        this->tn = tn;
        this->filename = filename;
        this->langname = lang;
        initFile();
    }

    language(bool r, bool comp, string filename, string lang)
    {
        this->comp = comp;
        if (r == 1)
        {
            this->r = true;
            this->comp = true;
        }
        else {
            this->r = r;
        }
        this->tn = 0;
        this->filename = filename;
        this->langname = lang;
        initFile();
    }

public:
    virtual ~language() {}
    virtual void compile() { }
    virtual void run() { }

protected:
    void initFile()
    {
        // if file exists -> use it (do not overwrite)
        if (FILE *file = fopen(filename.c_str(), "r"))
        {
            fclose(file);
            cout << "File " << filename << " already exists. Using existing source." << endl;
            return; // skip template copying
        }

        // otherwise, try to copy template
        string templateFile = "/usr/share/t/templates/" + langname + "/template" + to_string(tn);

        ifstream src(templateFile.c_str(), ios::binary);
        if (!src)
        {
            cerr << "Template not found: " << templateFile << endl;
            return;
        }

        ofstream dest(filename.c_str(), ios::binary | ios::trunc);
        dest << src.rdbuf();
        cout << "Template loaded from " << templateFile << " into " << filename << endl;
    }
};


class java : public language
{
public:
    java(bool runFlag, bool compileFlag, short tnum, string fname)
    : language(runFlag, compileFlag, tnum, fname, "java") {}
    java(bool runFlag, bool compileFlag, string fname)
    : language(runFlag, compileFlag, fname, "java") {}

    void compile() override
    {
        if (!comp) { cout << "Compile flag disabled for java; skipping compile." << endl; return; }
        cout << "Compiling " << filename << " with javac..." << endl;
        int status = system(("javac " + filename).c_str());
        decode_system_status(status, "javac");
    }

    void run() override
    {
        if (!r) return;
        // derive classname from filename (strip .java)
        string cls = filename;
        if (cls.size() > 5 && cls.substr(cls.size() - 5) == ".java")
            cls = cls.substr(0, cls.size() - 5);
        cout << "Running java " << cls << "..." << endl;
        int status = system(("java " + cls).c_str());
        decode_system_status(status, "java");
    }
};

class c : public language
{
    string outname;
public:
    c(bool runFlag, bool compileFlag, short tnum, string fname, string out)
    : language(runFlag, compileFlag, tnum, fname, "c"), outname(out) {}

    c(bool runFlag, bool compileFlag, short tnum, string fname)
    : language(runFlag, compileFlag, tnum, fname, "c")
    {
        outname = strip_extension(fname);
        if (outname.empty()) outname = "a.out";
    }

    c(bool runFlag, bool compileFlag, string fname, string out)
    : language(runFlag, compileFlag, fname, "c"), outname(out) {}

    c(bool runFlag, bool compileFlag, string fname)
    : language(runFlag, compileFlag, fname, "c")
    {
        outname = strip_extension(fname);
        if (outname.empty()) outname = "a.out";
    }

    void compile() override
    {
        if (!comp) { cout << "Compile flag disabled for c; skipping compile." << endl; return; }
        cout << "Compiling " << filename << " with gcc -o " << outname << "..." << endl;
        string cmd = "gcc " + filename + " -o " + outname + " 2>&1";
        int status = system(cmd.c_str());
        decode_system_status(status, "gcc");
    }

    void run() override
    {
        if (!r) return;
        cout << "Running " << outname << "..." << endl;
        int status = system(("./" + outname).c_str());
        decode_system_status(status, outname);
    }
};

class cpp : public language
{
    string outname;
public:
    cpp(bool runFlag, bool compileFlag, short tnum, string fname, string out)
    : language(runFlag, compileFlag, tnum, fname, "cpp"), outname(out) {}

    cpp(bool runFlag, bool compileFlag, short tnum, string fname)
    : language(runFlag, compileFlag, tnum, fname, "cpp")
    {
        outname = strip_extension(fname);
        if (outname.empty()) outname = "a.out";
    }

    cpp(bool runFlag, bool compileFlag, string fname, string out)
    : language(runFlag, compileFlag, fname, "cpp"), outname(out) {}

    cpp(bool runFlag, bool compileFlag, string fname)
    : language(runFlag, compileFlag, fname, "cpp")
    {
        outname = strip_extension(fname);
        if (outname.empty()) outname = "a.out";
    }

    void compile() override
    {
        if (!comp) { cout << "Compile flag disabled for cpp; skipping compile." << endl; return; }
        cout << "Compiling " << filename << " with g++ -o " << outname << "..." << endl;
        string cmd = "g++ " + filename + " -o " + outname + " 2>&1";
        int status = system(cmd.c_str());
        decode_system_status(status, "g++");
    }

    void run() override
    {
        if (!r) return;
        cout << "Running " << outname << "..." << endl;
        int status = system(("./" + outname).c_str());
        decode_system_status(status, outname);
    }
};

class python : public language
{
public:
    python(bool runFlag, bool compileFlagIgnored, short tnum, string fname)
    : language(runFlag, false, tnum, fname, "python") {}
    python(bool runFlag, bool compileFlagIgnored, string fname)
    : language(runFlag, false, fname, "python") {}

    void compile() override
    {
        cout << "Python is interpreted: compile ignored." << endl;
    }

    void run() override
    {
        if (!r) return;
        cout << "Running " << filename << " with python3..." << endl;
        int status = system(("python3 " + filename).c_str());
        decode_system_status(status, "python3");
    }
};

class rust : public language {
    string outname;
public:
    rust(bool runFlag, bool compileFlag, short tnum, string fname, string out)
    : language(runFlag, compileFlag, tnum, fname, "rust"), outname(out) {}

    rust(bool runFlag, bool compileFlag, short tnum, string fname)
    : language(runFlag, compileFlag, tnum, fname, "rust") {
        outname = strip_extension(fname);
        if (outname.empty()) outname = "a.out";
    }

    rust(bool runFlag, bool compileFlag, string fname, string out)
    : language(runFlag, compileFlag, fname, "rust"), outname(out) {}

    rust(bool runFlag, bool compileFlag, string fname)
    : language(runFlag, compileFlag, fname, "rust") {
        outname = strip_extension(fname);
        if (outname.empty()) outname = "a.out";
    }

    void compile() override {
        if (!comp) { cout << "Compile flag disabled for rust; skipping." << endl; return; }
        cout << "Compiling " << filename << " with rustc -o " << outname << "..." << endl;
        int status = system(("rustc " + filename + " -o " + outname).c_str());
        decode_system_status(status, "rustc");
    }

    void run() override {
        if (!r) return;
        cout << "Running " << outname << "..." << endl;
        int status = system(("./" + outname).c_str());
        decode_system_status(status, outname);
    }
};

class asm_lang : public language {
    string outname;
public:
    asm_lang(bool runFlag, bool compileFlag, short tnum, string fname, string out)
    : language(runFlag, compileFlag, tnum, fname, "asm"), outname(out) {}

    asm_lang(bool runFlag, bool compileFlag, short tnum, string fname)
    : language(runFlag, compileFlag, tnum, fname, "asm") {
        outname = strip_extension(fname);
        if (outname.empty()) outname = "a.out";
    }

    asm_lang(bool runFlag, bool compileFlag, string fname, string out)
    : language(runFlag, compileFlag, fname, "asm"), outname(out) {}

    asm_lang(bool runFlag, bool compileFlag, string fname)
    : language(runFlag, compileFlag, fname, "asm") {
        outname = strip_extension(fname);
        if (outname.empty()) outname = "a.out";
    }

    void compile() override {
        if (!comp) { cout << "Compile flag disabled for asm; skipping." << endl; return; }
        cout << "Assembling " << filename << " with nasm (elf64) and linking..." << endl;
        string obj = outname + ".o";
        int status = system((string("nasm -f elf64 ") + filename + " -o " + obj).c_str());
        decode_system_status(status, "nasm");
        if (status == 0) {
            status = system((string("ld ") + obj + " -o " + outname).c_str());
            decode_system_status(status, "ld");
        }
    }

    void run() override {
        if (!r) return;
        cout << "Running " << outname << "..." << endl;
        int status = system(("./" + outname).c_str());
        decode_system_status(status, outname);
    }
};

class fortran : public language {
    string outname;
public:
    fortran(bool runFlag, bool compileFlag, short tnum, string fname, string out)
        : language(runFlag, compileFlag, tnum, fname, "fortran"), outname(out) {}

        fortran(bool runFlag, bool compileFlag, short tnum, string fname)
            : language(runFlag, compileFlag, tnum, fname, "fortran") {
                outname = strip_extension(fname);
                if (outname.empty()) outname = "a.out";
            }

            fortran(bool runFlag, bool compileFlag, string fname, string out)
                : language(runFlag, compileFlag, fname, "fortran"), outname(out) {}

                fortran(bool runFlag, bool compileFlag, string fname)
                    : language(runFlag, compileFlag, fname, "fortran") {
                        outname = strip_extension(fname);
                        if (outname.empty()) outname = "a.out";
                    }

                    void compile() override {
                        if (!comp) { cout << "Compile flag disabled for fortran; skipping." << endl; return; }
                        cout << "Compiling " << filename << " with gfortran -o " << outname << "..." << endl;
                        int status = system(("gfortran " + filename + " -o " + outname).c_str());
                        decode_system_status(status, "gfortran");
                    }

                    void run() override {
                        if (!r) return;
                        cout << "Running " << outname << "..." << endl;
                        int status = system(("./" + outname).c_str());
                        decode_system_status(status, outname);
                    }
};

class dlang : public language {
    string outname;
public:
    dlang(bool runFlag, bool compileFlag, short tnum, string fname, string out)
    : language(runFlag, compileFlag, tnum, fname, "d"), outname(out) {}

    dlang(bool runFlag, bool compileFlag, short tnum, string fname)
    : language(runFlag, compileFlag, tnum, fname, "d") {
        outname = strip_extension(fname);
        if (outname.empty()) outname = "a.out";
    }

    dlang(bool runFlag, bool compileFlag, string fname, string out)
    : language(runFlag, compileFlag, fname, "d"), outname(out) {}

    dlang(bool runFlag, bool compileFlag, string fname)
    : language(runFlag, compileFlag, fname, "d") {
        outname = strip_extension(fname);
        if (outname.empty()) outname = "a.out";
    }

    void compile() override {
        if (!comp) { cout << "Compile flag disabled for D; skipping." << endl; return; }
        cout << "Compiling " << filename << " with dmd -of" << outname << "..." << endl;
        int status = system(("dmd " + filename + " -of" + outname).c_str());
        decode_system_status(status, "dmd");
    }

    void run() override {
        if (!r) return;
        cout << "Running " << outname << "..." << endl;
        int status = system(("./" + outname).c_str());
        decode_system_status(status, outname);
    }
};

class rlang : public language {
public:
    rlang(bool runFlag, bool compileFlagIgnored, short tnum, string fname)
    : language(runFlag, false, tnum, fname, "r") {}

    rlang(bool runFlag, bool compileFlagIgnored, string fname)
    : language(runFlag, false, fname, "r") {}

    void compile() override {
        cout << "R is interpreted: compile ignored." << endl;
    }

    void run() override {
        if (!r) return;
        cout << "Running " << filename << " with Rscript..." << endl;
        int status = system(("Rscript " + filename).c_str());
        decode_system_status(status, "Rscript");
    }
};
//R UNNER DESCRIPTIONS
void show_runner_versions() {
    cout << "=== Compiler/Interpreter Versions ===\n";
    // Java
    cout << "\n[java --version]\n";
    system("java --version");

    // Python
    cout << "\n[python3 --version]\n";
    system("python3 --version");

    // GCC
    cout << "\n[gcc --version]\n";
    system("gcc --version");

    // G++
    cout << "\n[g++ --version]\n";
    system("g++ --version");

    // Rust
    cout << "\n[rustc --version]\n";
    system("rustc --version");

    // Fortran
    cout << "\n[gfortran --version]\n";
    system("gfortran --version");

    // D
    cout << "\n[dmd --version]\n";
    system("dmd --version");

    // Assembly (nasm)
    cout << "\n[nasm --version]\n";
    system("nasm --version");

    // R
    cout << "\n[Rscript --version]\n";
    system("Rscript --version");
    cout << "=== End of Versions ===\n";
}

void print_usage()
{
    cout << "Usage: t [language] [options]\n"
    << "Languages: C, C++, Java, Python, Rust, ASM, Fortran, D, R\n"
    << "Options:\n"
    << "  -f, --file <name>       generated source filename (default main.c / Main.java / main.cpp / script.py)\n"
    << "  -o, --output <name>     output binary name for C/C++ (default: filename without extension)\n"
    << "  -c, --compile           enable compilation\n"
    << "  -r, --run               run after compile (implies -c)\n"
    << "  -t N, --template N      use template number N (templateN)\n"
    << "  -b, --bug               run bug/robustness tests (argument, random, stress, valgrind)\n" // NEW
    << "  -s, --supported         list supported languages\n"
    << "  -h, --help              show this help\n"
    << "  -v, --version           show version\n"
    << "  -rv                     display versions of installed compilers/interpreters\n" // NEW
    << "  -x  --cleanup           cleans up the executable files after running them\n"; //NEW
}
// Helper: check if file exists
bool file_exists(const string &fname)
{
    struct stat buffer;
    return stat(fname.c_str(), &buffer) == 0;
}

// Helper: run a system command and return exit code, printing status
int run_and_report(const string &cmd, const string &label)
{
    cout << "[TEST] " << label << " : " << cmd << endl;
    int status = system(cmd.c_str());
    int code = decode_system_status(status, label);
    return code;
}

// Helper: check if valgrind exists
bool have_valgrind()
{
    return system("which valgrind > /dev/null 2>&1") == 0;
}

// Helper: run bug tests
void run_bug_tests(const string &lang, const string &exename, const string &scriptname)
{
    cout << "\n=== BUG/ROBUSTNESS TESTS ===\n";
    vector<string> tests;
    vector<string> labels;
    int fails = 0, total = 0;

    bool is_compiled = (lang == "C" || lang == "CPP" || lang == "C++" || lang == "RUST" ||
    lang == "ASM" || lang == "FORTRAN" || lang == "D" || lang == "DLANG");
    bool is_script = (lang == "PYTHON" || lang == "PY" || lang == "R");

    string run_cmd;
    if (is_compiled) run_cmd = "./" + exename;
    else if (lang == "JAVA") run_cmd = "java " + strip_extension(scriptname);
    else if (lang == "PYTHON" || lang == "PY") run_cmd = "python3 " + scriptname;
    else if (lang == "R") run_cmd = "Rscript " + scriptname;
    else run_cmd = "";

    // Test the argument handling - Robi
    tests.push_back(run_cmd + " asd asd ads adsa");
    labels.push_back("Argument Handling");

    // 2. random small /dev/urandom data dump test
    tests.push_back("head -c 1024 /dev/urandom | " + run_cmd);
    labels.push_back("Random Input 1KB");

    // 3. if it handled the smaller data test, let's try the bigger data serts
    tests.push_back("head -c 65536 /dev/urandom | " + run_cmd);
    labels.push_back("Stress Input 64KB");

    // 4. Sample file test (if file 'sample.in' exists)
    if (file_exists("sample.in"))
    {
        tests.push_back(run_cmd + " < sample.in");
        labels.push_back("Sample Input File");
    }

    // 5. Valgrind test
    if (is_compiled && have_valgrind())
    {
        tests.push_back("valgrind --leak-check=full " + run_cmd + " < /dev/null");
        labels.push_back("Valgrind Leak Check");
    }

    // Run all tests
    for (size_t i = 0; i < tests.size(); ++i)
    {
        ++total;
        int code = run_and_report(tests[i], labels[i]);
        if (code == 0)
            cout << "[PASS] " << labels[i] << "\n";
        else
        {
            cout << "[FAIL] " << labels[i] << " (code " << code << ")\n";
            ++fails;
        }
    }
    cout << "=== BUG TEST SUMMARY: Passed ===" << (total-fails) << "/" << total << " ===\n\n";
}
// If the user has fun, why not automate the cleaning process?
void cleanup_file(const string &fname)
{
    if (remove(fname.c_str()) == 0)
        cout << "[CLEANUP] Deleted: " << fname << endl;
    else
        cout << "[CLEANUP] File not found (skipped): " << fname << endl;
}
//Java:
void cleanup_java_classes(const string &base)
{
    cout << "[CLEANUP] Deleting Java .class files..." << endl;
    // remove all .class files in current directory
    int code = system("rm -f *.class");
    if (code == 0)
        cout << "[CLEANUP] All .class files removed." << endl;
    else
        cout << "[CLEANUP] rm returned code " << code << endl;
}
//Prompt user:
bool prompt_cleanup(const string &desc)
{
    cout << "[CLEANUP] " << desc << " Proceed? [y/N]: ";
    string resp;
    getline(cin, resp);
    return !resp.empty() && (resp[0] == 'y' || resp[0] == 'Y');
}



int main(int argc, char* argv[])
{
    string supp_lang[9] = {"C","C++","JAVA","PYTHON","RUST","ASM","FORTRAN","D","R"};

    if (argc == 1)
    {
        print_usage();
        return 0;
    }
    string first = argv[1];
    string U = to_upper(first);
    if (U == "-RV") {
        show_runner_versions();
        return 0;
    }
    bool cleanupFlag = false;
    bool promptCleanupFlag = false;
    bool compileFlag = false;
    bool runFlag = false;
    bool bugFlag = false;
    short tnum = 0; // default template 0
    bool tnum_specified = false;
    string filename_override = ""; // -f
    string out_override = "";      // -o


    if (U == "-H" || U == "--HELP") { print_usage(); return 0; }
    if (U == "-S" || U == "--SUPPORTED") {
        cout << "Supported languages:\n";
        for (int i=0;i<9;i++) cout << "  " << supp_lang[i] << "\n";
        return 0;
    }
    if (U == "-V" || U == "--VERSION") {
        cout << "t version 0.2\n";
        return 0;
    }

    // Parse flags, robustly and case-insensitive
    for (int i = 2; i < argc; ++i)
    {
        string a = argv[i];
        string A = to_upper(a);

        if (A == "-C" || A == "--COMPILE") compileFlag = true;
        else if (A == "-R" || A == "--RUN") { runFlag = true; compileFlag = true; }
        else if (A == "-T" || A == "--TEMPLATE") {
            if (i+1 < argc) {
                tnum = (short) atoi(argv[++i]);
                tnum_specified = true;
            } else {
                cerr << "Missing template number after -t\n";
                return 1;
            }
        } else if (A == "-F" || A == "--FILE") {
            if (i+1 < argc) {
                filename_override = argv[++i];
            } else {
                cerr << "Missing filename after -f\n";
                return 1;
            }
        } else if (A == "-O" || A == "--OUTPUT") {
            if (i+1 < argc) {
                out_override = argv[++i];
            } else {
                cerr << "Missing output name after -o\n";
                return 1;
            }
        } else if (A == "-B" || A == "--BUG") bugFlag = true;
        else if (A == "--CLEANUP" || A == "-X") cleanupFlag = true;
        else if (A == "--PROMPT-CLEANUP") promptCleanupFlag = true;
        else if (A == "-H" || A == "--HELP") { print_usage(); return 0; }
        else if (A == "-V" || A == "--VERSION") { cout << "t version 0.2\n"; return 0; }
        else if (A == "-R" || A == "-C") { /* Already handled - ignore */ }
        else if (A == "-F" || A == "-O") { /* Already handled - ignore */ }
        else if (A == "-B" || A == "-X") { /* Already handled - ignore */ }
        else {
            cerr << "Unknown option: " << argv[i] << "\n";
            return 1;
        }
    }

    // choose filename defaults
    string default_fname;
    if (U == "C") default_fname = "main.c";
    else if (U == "CPP" || U == "C++") default_fname = "main.cpp";
    else if (U == "JAVA") default_fname = "Main.java";
    else if (U == "PYTHON" || U == "PY") default_fname = "script.py";
    else if (U == "RUST") default_fname = "main.rs";
    else if (U == "ASM" || U == "ASSEMBLY") default_fname = "main.asm";
    else if (U == "FORTRAN") default_fname = "main.f90";
    else if (U == "D" || U == "DLANG") default_fname = "main.d";
    else if (U == "R") default_fname = "script.R";
    else { cerr << "Unsupported language: " << first << "\n"; return 1; }

    string chosen_fname = filename_override.empty() ? default_fname : filename_override;

    bool file_exists_flag = file_exists(chosen_fname);

    if (file_exists_flag && !compileFlag && !runFlag) {
        if (U == "C" || U == "CPP" || U == "C++" || U == "JAVA" || U == "RUST" || U == "ASM" || U == "FORTRAN" || U == "D" || U == "DLANG") {
            compileFlag = true;
            runFlag = true;
            cout << "Detected existing source. Auto-enabling compile + run." << endl;
        } else if (U == "PYTHON" || U == "PY" || U == "R") {
            runFlag = true;
            cout << "Detected existing script. Auto-enabling run." << endl;
        }
    }

    language* prog = nullptr;

    if (U == "C" )
    {
        if (!out_override.empty()) {
            if (tnum_specified) prog = new c(runFlag, compileFlag, tnum, chosen_fname, out_override);
            else prog = new c(runFlag, compileFlag, chosen_fname, out_override);
        } else {
            if (tnum_specified) prog = new c(runFlag, compileFlag, tnum, chosen_fname);
            else prog = new c(runFlag, compileFlag, chosen_fname);
        }
    }
    else if (U == "CPP" || U == "C++")
    {
        if (!out_override.empty()) {
            if (tnum_specified) prog = new cpp(runFlag, compileFlag, tnum, chosen_fname, out_override);
            else prog = new cpp(runFlag, compileFlag, chosen_fname, out_override);
        } else {
            if (tnum_specified) prog = new cpp(runFlag, compileFlag, tnum, chosen_fname);
            else prog = new cpp(runFlag, compileFlag, chosen_fname);
        }
    }
    else if (U == "JAVA")
    {
        if (tnum_specified) prog = new java(runFlag, compileFlag, tnum, chosen_fname);
        else prog = new java(runFlag, compileFlag, chosen_fname);
    }
    else if (U == "PYTHON" || U == "PY")
    {
        if (tnum_specified) prog = new python(runFlag, compileFlag, tnum, chosen_fname);
        else prog = new python(runFlag, compileFlag, chosen_fname);
    }
    else if (U == "RUST") {
        if (!out_override.empty()) {
            if (tnum_specified) prog = new rust(runFlag, compileFlag, tnum, chosen_fname, out_override);
            else prog = new rust(runFlag, compileFlag, chosen_fname, out_override);
        } else {
            if (tnum_specified) prog = new rust(runFlag, compileFlag, tnum, chosen_fname);
            else prog = new rust(runFlag, compileFlag, chosen_fname);
        }
    }
    else if (U == "ASM" || U == "ASSEMBLY") {
        if (!out_override.empty()) {
            if (tnum_specified) prog = new asm_lang(runFlag, compileFlag, tnum, chosen_fname, out_override);
            else prog = new asm_lang(runFlag, compileFlag, chosen_fname, out_override);
        } else {
            if (tnum_specified) prog = new asm_lang(runFlag, compileFlag, tnum, chosen_fname);
            else prog = new asm_lang(runFlag, compileFlag, chosen_fname);
        }
    }
    else if (U == "FORTRAN") {
        if (!out_override.empty()) {
            if (tnum_specified) prog = new fortran(runFlag, compileFlag, tnum, chosen_fname, out_override);
            else prog = new fortran(runFlag, compileFlag, chosen_fname, out_override);
        } else {
            if (tnum_specified) prog = new fortran(runFlag, compileFlag, tnum, chosen_fname);
            else prog = new fortran(runFlag, compileFlag, chosen_fname);
        }
    }
    else if (U == "D" || U == "DLANG") {
        if (!out_override.empty()) {
            if (tnum_specified) prog = new dlang(runFlag, compileFlag, tnum, chosen_fname, out_override);
            else prog = new dlang(runFlag, compileFlag, chosen_fname, out_override);
        } else {
            if (tnum_specified) prog = new dlang(runFlag, compileFlag, tnum, chosen_fname);
            else prog = new dlang(runFlag, compileFlag, chosen_fname);
        }
    }
    else if (U == "R") {
        if (tnum_specified) prog = new rlang(runFlag, compileFlag, tnum, chosen_fname);
        else prog = new rlang(runFlag, compileFlag, chosen_fname);
    }

    if (!prog) { cerr << "Failed to create language object" << endl; return 1; }

    prog->compile();
    prog->run();

    // --- BUG MODE ---
    if (bugFlag)
    {
        string outname = chosen_fname;
        if (U == "C" || U == "CPP" || U == "C++" || U == "RUST" ||
            U == "ASM" || U == "FORTRAN" || U == "D" || U == "DLANG")
        {
            // guess output binary name
            if (!out_override.empty()) outname = out_override;
            else outname = strip_extension(chosen_fname);
            if (outname.empty()) outname = "a.out";
        }
        run_bug_tests(U, outname, chosen_fname);
    }

    // --- CLEANUP MODE ---
    if (cleanupFlag)
    {
        bool do_cleanup = true;
        if (promptCleanupFlag)
            do_cleanup = prompt_cleanup("Cleanup generated files?");

        if (do_cleanup)
        {
            if (U == "C" || U == "CPP" || U == "C++" || U == "RUST" ||
                U == "ASM" || U == "FORTRAN" || U == "D" || U == "DLANG")
            {
                string outname = out_override.empty() ? strip_extension(chosen_fname) : out_override;
                if (outname.empty()) outname = "a.out";
                cleanup_file(outname);
            }
            else if (U == "JAVA")
            {
                cleanup_java_classes(strip_extension(chosen_fname));
            }
            // else: interpreted languages don't have output files to remove
        }
        else
        {
            cout << "[CLEANUP] Skipped by user.\n";
        }
    }

    delete prog;
    return 0;
}
