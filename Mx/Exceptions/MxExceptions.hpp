#pragma once

#ifndef MX_EXCEPTION_HPP_
#define MX_EXCEPTION_HPP_

#include <stdexcept>
#include <string>

#define MX_DECLARE_RUNTIME_ERROR(errorName, errorMsg) \
class errorName final : public std::runtime_error {\
public:\
    explicit errorName() : std::runtime_error(#errorMsg) {}\
};

namespace Mix {
    class FactoryFunctionRepetitionException : public std::exception {
    public:
        explicit FactoryFunctionRepetitionException(const std::string& _className) {
            mWhat = "[ERROR] Attempting to re-register factory function of type [" + _className + "]";
        }

        explicit FactoryFunctionRepetitionException(const char* _className) {
            mWhat = "[ERROR] Attempting to re-register factory function of type [" + std::string(_className) + "]";
        }

        const char* what() const override {
            return mWhat.c_str();
        }

    private:
        std::string mWhat;
    };

    MX_DECLARE_RUNTIME_ERROR(ComponentCastingError, [ERROR] Cannot cast type to Component)

    class IndependentComponentError final : public std::runtime_error {
    public:
        explicit IndependentComponentError(const std::string& _name) : std::runtime_error("[ERROR] Component [" + _name + "] not attached to a GameObject") {}
    };

    MX_DECLARE_RUNTIME_ERROR(SdlInitializationError, [ERROR] Failed to initialize SDL2)

    MX_DECLARE_RUNTIME_ERROR(WindowCreationError, [ERROR] Failed to create window)

    MX_DECLARE_RUNTIME_ERROR(WindowIconLoadingError, [ERROR] Failed to load icon image)

    MX_DECLARE_RUNTIME_ERROR(FmodInitializationError, [ERROR] Failed to initialize FMOD)
}

#endif
