#pragma once
#ifndef _MX_EXCEPTION_HPP_
#define _MX_EXCEPTION_HPP_

#include<string>
#include<stdexcept>

namespace Mix {

    class RepeatFactoryFuncExcep :public std::exception {
    private:
        std::string mWhat;
    public:
        explicit RepeatFactoryFuncExcep(const std::string& className) {
            mWhat = "Tring to register FactoryFunction that has been registered. Type name:[" + className + "]";
        }

        explicit RepeatFactoryFuncExcep(const char* className) {
            mWhat += "Tring to register FactoryFunction that has been registered. Type name:["
                + (className + std::string("]"));
        }

        const char* what() const override {
            return mWhat.c_str();
        }
    };

}

#endif // !_MX_EXCEPTION_HPP_
