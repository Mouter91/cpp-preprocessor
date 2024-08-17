#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

bool ProcessRecursInclude(ifstream& input, ofstream& output, const path& current_file, const vector<path>& include_directories) {
    string line;
    int line_num = 1;
    regex include_pattern_angle(R"(\s*#\s*include\s*<([^>]*)>\s*)");
    regex include_pattern_quotes(R"(\s*#\s*include\s*\"([^"]*)\"\s*)");

    while (getline(input, line)) {
        smatch match;
        path path_ex;
        if (regex_search(line, match, include_pattern_quotes)) {
            path path_inc(match[1].str());
            path_ex = current_file.parent_path() / path_inc;
            bool found_file = exists(path_ex);
            if (!found_file) {
                for (const auto& dir : include_directories) {
                    path path_pot = dir / path_inc;
                    if (exists(path_pot)) {
                        path_ex = path_pot;
                        found_file = true;
                        break;
                    }
                }
            }
            if (!found_file) {
                cout <<  "unknown include file " << path_inc.string()
                     << " at file " << current_file.string()
                     << " at line " << line_num << endl;
                return false;
            }
        }
        else if (regex_search(line, match, include_pattern_angle)) {
            path path_inc(match[1].str());
            path_ex = current_file.parent_path() / path_inc;
            bool found_file = exists(path_ex);
            if (!found_file) {
                for (const auto& dir : include_directories) {
                    path path_pot = dir / path_inc;
                    if (exists(path_pot)) {
                        path_ex = path_pot;
                        found_file = true;
                        break;
                    }
                }
            }
            if (!found_file) {
                cout <<  "unknown include file " << path_inc.string()
                     << " at file " << current_file.string()
                     << " at line " << line_num << endl;
                return false;
            }
        }
        else {
            output << line << endl;
            ++line_num;
            continue;
        }

        ifstream file_include(path_ex);
        if (!file_include.is_open()) {
            cout << "Failed to open file " << path_ex.string()
                 << " at file " << current_file.string()
                 << " at line " << line_num << endl;
            return false;
        }

        if (!ProcessRecursInclude(file_include, output, path_ex, include_directories)) {
            return false;
        }

        ++line_num;
    }
    return true;
}

bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories) {
    ifstream input_file(in_file);
    if(!input_file.is_open()) {
        return false;
    }

    ofstream output_file(out_file);
    if(!output_file.is_open()) {
        return false;
    }

    if(!ProcessRecursInclude(input_file, output_file, in_file, include_directories)) {
        output_file.close();
        return false;
    }

    return true;
}

string GetFileContents(string file) {
    ifstream stream(file);
    return {(istreambuf_iterator<char>(stream)), istreambuf_iterator<char>()};
}

void Test() {
    error_code err;
    filesystem::remove_all("sources"_p, err);
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err);
    filesystem::create_directories("sources"_p / "include1"_p, err);
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err);

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
                "#include \"dir1/b.h\"\n"
                "// text between b.h and c.h\n"
                "#include \"dir1/d.h\"\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"
                "#   include<dummy.txt>\n"
                "}\n"s;
    }
    {
        ofstream file("sources/dir1/b.h");
        file << "// text from b.h before include\n"
                "#include \"subdir/c.h\"\n"
                "// text from b.h after include"s;
    }
    {
        ofstream file("sources/dir1/subdir/c.h");
        file << "// text from c.h before include\n"
                "#include <std1.h>\n"
                "// text from c.h after include\n"s;
    }
    {
        ofstream file("sources/dir1/d.h");
        file << "// text from d.h before include\n"
                "#include \"lib/std2.h\"\n"
                "// text from d.h after include\n"s;
    }
    {
        ofstream file("sources/include1/std1.h");
        file << "// std1\n"s;
    }
    {
        ofstream file("sources/include2/lib/std2.h");
        file << "// std2\n"s;
    }

    assert(Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p,
                      {"sources"_p / "include1"_p,"sources"_p / "include2"_p}));

    ostringstream test_out;
    test_out << "// this comment before include\n"
                "// text from b.h before include\n"
                "// text from c.h before include\n"
                "// std1\n"
                "// text from c.h after include\n"
                "// text from b.h after include\n"
                "// text between b.h and c.h\n"
                "// text from d.h before include\n"
                "// std2\n"
                "// text from d.h after include\n"
                "\n"
                "int SayHello() {\n"
                "    cout << \"hello, world!\" << endl;\n"s;

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}

int main() {
    Test();
}
