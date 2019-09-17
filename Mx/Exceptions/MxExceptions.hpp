#pragma once

#ifndef MX_EXCEPTION_HPP_
#define MX_EXCEPTION_HPP_

#include "../Utils/MxUtils.h"
#include <stdexcept>
#include <string>
#include <utility>

#define MX_DECLARE_RUNTIME_ERROR(errorName, errorMsg) \
class errorName final : public std::runtime_error {\
public:\
    explicit errorName() : std::runtime_error(#errorMsg) {}\
};

namespace Mix {

    class Exception : std::exception {
    public:
        template<typename ... _Args>
        explicit Exception(const std::string& _format, _Args&&... _args) {
            mWhat = Utils::StringFormat(_format, std::forward<_Args>(_args)...);
        }

        explicit Exception(std::string _str) : mWhat(std::move((_str))) {}

        char const* what() const override {
            return mWhat.c_str();
        }

    private:
        std::string mWhat;
    };

    MX_DECLARE_RUNTIME_ERROR(ComponentCastingError, [ERROR] Cannot cast type to Component)

        class IndependentComponentError final : public std::runtime_error {
        public:
            explicit IndependentComponentError(const std::string& _name)
                : std::runtime_error("[ERROR] Component [" + _name + "] is not attached to a GameObject") {}
    };

    class ThirdPartyLibInitError final : public std::runtime_error {
    public:
        explicit ThirdPartyLibInitError(const std::string& _libName)
            : std::runtime_error("[ERROR] Failed to initialize " + _libName) {}
    };

    MX_DECLARE_RUNTIME_ERROR(WindowCreationError, [ERROR] Failed to create window)

        MX_DECLARE_RUNTIME_ERROR(WindowIconLoadingError, [ERROR] Failed to load icon image)

#ifndef MX_EXCEPT
#define MX_EXCEPT(desc) \
    {\
        throw Exception("- Error: %1%\n- In Function: %2%\n- In File %3% : %4%", desc, __FUNCTION__, __FILE__, __LINE__);\
    }
}

#endif

#endif
