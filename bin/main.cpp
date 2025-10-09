#include <iostream>
#include <cstring>
#include <fstream>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/wait.h>
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
        return 128 + sig; // common convention
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
            cout << "File " << filename << " already exists. Using existing source.\n";
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
        if (!comp) { cout << "Compile flag disabled for java; skipping compile.\n"; return; }
        cout << "Compiling " << filename << " with javac...\n";
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
        cout << "Running java " << cls << "...\n";
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
        if (!comp) { cout << "Compile flag disabled for c; skipping compile.\n"; return; }
        cout << "Compiling " << filename << " with gcc -o " << outname << "...\n";
        string cmd = "gcc " + filename + " -o " + outname + " 2>&1";
        int status = system(cmd.c_str());
        decode_system_status(status, "gcc");
    }

    void run() override
    {
        if (!r) return;
        cout << "Running " << outname << "...\n";
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
        if (!comp) { cout << "Compile flag disabled for cpp; skipping compile.\n"; return; }
        cout << "Compiling " << filename << " with g++ -o " << outname << "...\n";
        string cmd = "g++ " + filename + " -o " + outname + " 2>&1";
        int status = system(cmd.c_str());
        decode_system_status(status, "g++");
    }

    void run() override
    {
        if (!r) return;
        cout << "Running " << outname << "...\n";
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
        cout << "Python is interpreted: compile ignored.\n";
    }

    void run() override
    {
        if (!r) return;
        cout << "Running " << filename << " with python3...\n";
        int status = system(("python3 " + filename).c_str());
        decode_system_status(status, "python3");
    }
};

/* ---------- usage/help ---------- */

void print_usage()
{
    cout << "Usage: t [language] [options]\n"
    << "Languages: C, C++, Java, Python\n"
    << "Options:\n"
    << "  -f, --file <name>       generated source filename (default main.c / Main.java / main.cpp / script.py)\n"
    << "  -o, --output <name>     output binary name for C/C++ (default: filename without extension)\n"
    << "  -c, --compile           enable compilation\n"
    << "  -r, --run               run after compile (implies -c)\n"
    << "  -t N, --template N      use template number N (templateN)\n"
    << "  -s, --supported         list supported languages\n"
    << "  -h, --help              show this help\n"
    << "  -v, --version           show version\n";
}


int main(int argc, char* argv[])
{
    string supp_lang[4] = {"C","C++","JAVA","PYTHON"};

    if (argc == 1)
    {
        print_usage();
        return 0;
    }

    string first = argv[1];
    string U = to_upper(first);

    if (U == "-H" || U == "--HELP") { print_usage(); return 0; }
    if (U == "-S" || U == "--SUPPORTED") {
        cout << "Supported languages:\n";
        for (int i=0;i<4;i++) cout << "  " << supp_lang[i] << "\n";
        return 0;
    }
    if (U == "-V" || U == "--VERSION") {
        cout << "t version 0.2\n";
        return 0;
    }

    // parse flags
    bool compileFlag = false;
    bool runFlag = false;
    short tnum = 0; // default template 0
    bool tnum_specified = false;
    string filename_override = ""; // -f
    string out_override = "";      // -o

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
        } else if (A == "-F" || A == "--FILE" || A == "-f" ) {
            if (i+1 < argc) {
                filename_override = argv[++i];
            } else {
                cerr << "Missing filename after -f\n";
                return 1;
            }
        } else if (A == "-O" || A == "--OUTPUT" || A == "-o") {
            if (i+1 < argc) {
                out_override = argv[++i];
            } else {
                cerr << "Missing output name after -o\n";
                return 1;
            }
        } else if (A == "-H" || A == "--HELP") { print_usage(); return 0; }
        else if (A == "-V" || A == "--VERSION") { cout << "t version 0.2\n"; return 0; }
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
    else { cerr << "Unsupported language: " << first << "\n"; return 1; }

    string chosen_fname = filename_override.empty() ? default_fname : filename_override;

    // if file exists and user didn't give -c/-r -> auto-enable sensible behavior
    bool file_exists = false;
    if (FILE *f = fopen(chosen_fname.c_str(), "r")) { fclose(f); file_exists = true; }

    if (file_exists && !compileFlag && !runFlag) {
        if (U == "C" || U == "CPP" || U == "C++" || U == "JAVA") {
            compileFlag = true;
            runFlag = true;
            cout << "Detected existing source. Auto-enabling compile + run.\n";
        } else if (U == "PYTHON" || U == "PY") {
            runFlag = true;
            cout << "Detected existing script. Auto-enabling run.\n";
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

    if (!prog) { cerr << "Failed to create language object\n"; return 1; }

    prog->compile();
    prog->run();

    delete prog;
    return 0;
}
