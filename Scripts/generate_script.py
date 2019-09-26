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
    file = open("./" + name + ".h", "w")
    file.write("""#pragma once

#include "../Mx/Component/Script/MxScript.h"

namespace Scripts {{
    using namespace Mix;

    class {name} final : public Script {{
    MX_DECLARE_RTTI

    public:
        {name}() = default;
        ~{name}() = default;

    private:
        void start() override;
        void update() override;
    }};
}}
""".format(name=name))
    file.close()

    # c++ file
    file = open("./" + name + ".cpp", "w")
    file.write("""#include "{name}.h"

namespace Scripts {{
    MX_IMPLEMENT_RTTI({name}, Script)

    void {name}::start() {{}}

    void {name}::update() {{}}
}}
""".format(name=name))
    file.close()


def print_usage():
    print("""usage: generate.py [-option] [args...]
-h --help            : print this help message
-s <classname>
--script <classname> : generate <classname>.cpp and <classname>.h in the same folder
""")


def main(argv):
    if len(argv) == 0:
        print_usage()
        sys.exit()
    # todo: add -o --output
    try:
        args, _ = getopt.getopt(argv, "hs:", ["help", "script="])
    except getopt.GetoptError:
        print_usage()
        sys.exit()
    for opt, val in args:
        if opt in ("-h", "--help"):
            print_usage()
        elif opt in ("-s", "--script"):
            generate_script(val)


if __name__ == "__main__":
    main(sys.argv[1:])
