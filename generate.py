import getopt
import os
import re
import sys


def assert_valid(name):
    if not re.match("^[a-zA-Z_][a-zA-Z0-9_]*$", name):
        print(name + " is an invalid C++ identifier.")
        sys.exit()


def generate_script(name):
    assert_valid(name)

    # header file
    file = open("./Scripts/" + name + ".h", "w")
    file.write("""#pragma once

#ifndef _{name}_H_
#define _{name}_H_

#include "../MxBehaviour.h"

class {name} : public Mix::MxBehaviour {{
public:
    {name}();
    ~{name}();
    void init();
    void update();
}};

#endif
""".format(name=name))
    file.close()

    # c++ file
    file = open("./Scripts/" + name + ".cpp", "w")
    file.write("""#include "{name}.h"

{name}::{name}() {{}}

{name}::~{name}() {{}}

void {name}::init() {{}}

void {name}::update() {{}}
""".format(name=name))
    file.close()


def get_names():
    names = os.listdir("./Scripts")
    for i in range(len(names)):
        names[i] = names[i].rpartition(".")[0]
        assert_valid(names[i])
    return set(names)


def generate_main(names):
    file = open("main.cpp", "w")
    code = '#include "MxApplication.h"\n\n'
    for name in names:
        code += '#include "./Scripts/{name}.h"\n'.format(name=name)
    code += '\nint main() {\n    Mix::MxApplication app;\n\n'
    # todo: link scripts with objects in the scene
    for name in names:
        code += '    app.behaviours.push_back(std::make_shared<{name}>());\n'.format(name=name)
    code += '\n    return app.execute();\n}\n'
    file.write(code)
    file.close()


def print_usage():
    print("""usage: generate.py [-option] [args...]
-h --help            : print this help message
-m --main            : generate main.cpp
-s <classname>
--script <classname> : generate <classname>.cpp and <classname>.h in ./Scripts
""")


def main(argv):
    if len(argv) == 0:
        print_usage()
        sys.exit()
    try:
        args, _ = getopt.getopt(argv, "hms:", ["help", "main", "script="])
    except getopt.GetoptError:
        print_usage()
        sys.exit()
    for opt, val in args:
        if opt in ("-h", "--help"):
            print_usage()
        elif opt in ("-m", "--main"):
            generate_main(get_names())
            # todo: create CMake files
        elif opt in ("-s", "--script"):
            generate_script(val)


if __name__ == "__main__":
    main(sys.argv[1:])
