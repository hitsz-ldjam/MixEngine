#pragma once

#ifndef MX_EXCEPTION_HPP
#define MX_EXCEPTION_HPP

#include <string>
#include <stdexcept>

#define MX_DECLARE_RUNTIME_ERROR(errorName, errorMsg) \
class errorName : public std::runtime_error {\
public:\
    errorName() : std::runtime_error(#errorMsg) {}\
};

namespace Mix {
    class FactoryFunctionRepetitionException final : public std::exception {
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

    MX_DECLARE_RUNTIME_ERROR(SDLInitializationError, [ERROR] Failed to initialize SDL)

    MX_DECLARE_RUNTIME_ERROR(WindowCreationError, [ERROR] Failed to create window)

    MX_DECLARE_RUNTIME_ERROR(WindowIconLoadingError, [ERROR] Failed to load icon image)

    MX_DECLARE_RUNTIME_ERROR(FMODInitializationError, [ERROR] Failed to initialize FMOD)
}

#endif
