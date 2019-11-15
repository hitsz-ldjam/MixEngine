#pragma once

#ifndef MX_EXCEPTION_HPP_
#define MX_EXCEPTION_HPP_

#include "../Utils/MxUtils.h"
#include <stdexcept>
#include <string>

namespace Mix {
    class Exception : std::exception {
    public:
        template<typename... _Args>
        explicit Exception(const std::string& _format, _Args&&... _args) {
            mWhat = Utils::StringFormat(_format, std::forward<_Args>(_args)...);
        }

        explicit Exception(std::string _str) : mWhat(std::move(_str)) {}

        virtual ~Exception() = default;

        char const* what() const override {
            return mWhat.c_str();
        }

    private:
        std::string mWhat;
    };

#ifndef MX_EXCEPT
#define MX_EXCEPT(desc) \
    { throw Exception("- Error: %1%\n- In Function: %2%\n- In File %3% : %4%", desc, __FUNCTION__, __FILE__, __LINE__); }
#endif
}

#endif
