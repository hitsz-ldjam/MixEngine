import getopt
import os
import re
import sys


str_template = {
    "help":
"""usage: editor_prototype.py [-option] [args...]

note: The following options are also available in interactive mode.

no option         : Enter interactive mode.

-h --help         : Print this help message.

-p <name>
--project <name>  : Open project by name. Create a new one if not exist.

-q --quit         : Quit editor.

-s <name>
--script <name>   : Create new script by name. Project should be specified in advance.
""",

    "main.cpp":
"""#include "../MixEngine.h"
#include <SDL2/SDL_main.h>

#include "{proj_name}.h"

extern "C" int main(int argc, char** argv) {{
    Mix::MixEngine::Initialize("Mix", argc, argv);
    return Mix::MixEngine::Instance().execute(std::make_shared<{proj_name}>());
}}
""",

    "project.h":
"""#pragma once

#ifndef {proj_name_upper}_H_
#define {proj_name_upper}_H_

#include "../MxApplicationBase.h"

using namespace Mix;

class {proj_name} final : public ApplicationBase {{
public:
    std::string getAppName() const override;
    Version getAppVersion() const override;

private:
    void onMainSceneCreated() override;
}};

#endif
""",

    "project.cpp":
"""#include "{proj_name}.h"
#include "../Mx/GameObject/MxGameObject.h"

std::string {proj_name}::getAppName() const {{ return std::string("{proj_name}"); }}

Version {proj_name}::getAppVersion() const {{ return Version(1, 0, 0); }}

void {proj_name}::onMainSceneCreated() {{
    
}}
""",

    "script.h":
"""#pragma once

#ifndef {script_name_upper}_H_
#define {script_name_upper}_H_

#include "../../../../Mx/Component/Script/MxScript.h"

using namespace Mix;

class {script_name} final : public Script {{
MX_DECLARE_RTTI

public:
    {script_name}() = default;
    ~{script_name}() = default;

private:
    void start() override;
    void update() override;
}};

using H{script_name} = SceneObjectHandle<{script_name}>;

#endif
""",

    "script.cpp":
"""#include "{script_name}.h"

MX_IMPLEMENT_RTTI({script_name}, Script)

void {script_name}::start() {{
    
}}

void {script_name}::update() {{
    
}}
"""
}


def check_validation(cpp_idf):
    return re.match("^[a-zA-Z_][a-zA-Z0-9_]*$", cpp_idf)


def write_if_not_exist(path, string):
    if os.path.exists(path):
        if os.path.isfile(path):
            print("file '{path}' already exists".format(path=path))
        else:
            print("file name '{path}' occupied by a directory".format(path=path))
        return False
    try:
        file = open(path, "w")
        file.write(string)
        file.close()
    except Exception as e:
        print(str(e))
        return False
    print("create file '{path}'".format(path=path))
    return True


def help_impl():
    global str_template
    print(str_template["help"])


def project_impl(proj_name):
    if not check_validation(proj_name):
        print("invalid project name '{proj_name}'".format(proj_name=proj_name))
        return ""

    os.makedirs("./" + proj_name, exist_ok=True)

    global str_template

    # .h
    write_if_not_exist("./{proj_name}/{proj_name}.h".format(proj_name=proj_name),
                       str_template["project.h"].format(proj_name=proj_name, proj_name_upper=proj_name.upper()))

    # .cpp
    write_if_not_exist("./{proj_name}/{proj_name}.cpp".format(proj_name=proj_name),
                       str_template["project.cpp"].format(proj_name=proj_name))

    # main.cpp
    write_if_not_exist("./{proj_name}/main.cpp".format(proj_name=proj_name),
                       str_template["main.cpp"].format(proj_name=proj_name))

    print("open project '{proj_name}'".format(proj_name=proj_name))
    return proj_name


def script_impl(proj_name, script_name):
    if not check_validation(script_name):
        print("invalid script name '{script_name}'".format(script_name=script_name))
        return
    if not proj_name:
        print("project not specified for script '{script_name}'".format(script_name=script_name))
        return

    # todo: specify output path
    script_path = "./{proj_name}/Assets/Scripts/{script_name}".format(proj_name=proj_name, script_name=script_name)
    os.makedirs(script_path, exist_ok=True)

    global str_template

    # .h
    write_if_not_exist("{script_path}/{script_name}.h".format(script_path=script_path, script_name=script_name),
                       str_template["script.h"].format(script_name=script_name, script_name_upper=script_name.upper()))

    # .cpp
    write_if_not_exist("{script_path}/{script_name}.cpp".format(script_path=script_path, script_name=script_name),
                       str_template["script.cpp"].format(script_name=script_name))

    print("create script '{script_name}' in project '{proj_name}'".format(proj_name=proj_name, script_name=script_name))


def main(argv):
    active_proj = ""

    if not len(argv):
        while True:
            commands = input().split()
            if not len(commands):
                continue

            # func_dict.get(commands[0], lambda *_: print("invalid command"))(*commands[1:])

            opt = commands[0]
            if opt in ("h", "help"):
                help_impl()
            elif opt in ("p", "project"):
                if len(commands) == 2:
                    active_proj = project_impl(commands[1])
                else:
                    active_proj = ""
                    print("invalid syntax")
            elif opt in ("s", "script"):
                if len(commands) == 2:
                    script_impl(active_proj, commands[1])
                else:
                    print("invalid syntax")
            elif opt in ("q", "quit"):
                break
            else:
                print("invalid command")
            print()

    else:
        # todo: support multiple arguments
        try:
            args, _ = getopt.getopt(argv, "hp:qs:", ["help", "project=", "quit", "script="])
        except getopt.GetoptError:
            print("invalid command")
            return
        for opt, val in args:
            if opt in ("-h", "--help"):
                help_impl()
                break
            elif opt in ("-p", "--project"):
                active_proj = project_impl(val)
            elif opt in ("-s", "--script"):
                script_impl(active_proj, val)
            elif opt in ("-q", "--quit"):
                break


if __name__ == "__main__":
    main(sys.argv[1:])

