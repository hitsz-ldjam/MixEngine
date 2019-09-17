#pragma once
#ifndef MX_LOG_H_
#define MX_LOG_H_

#include "../Utils/MxUtils.h"
#include "../Utils/MxFlags.h"
#include "../Definitions/MxDefinitions.h"

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <mutex>

namespace Mix {
    enum class OutputMode {
        Console = 0b001,
        File = 0b010,
    };
    MX_ALLOW_FLAGS_FOR_ENUM(OutputMode);

    class Log {
    public:

        static void SetOutputMode(Flags<OutputMode> _out) {
            std::lock_guard<std::mutex> lock(sMutex);
            sOutputMode = _out;
        }

        static Flags<OutputMode> GetOutputMode() {
            return sOutputMode;
        }

        template<typename... _Args>
        static void Info(const std::string& _format, _Args... _args) {
            Info(Utils::StringFormat(_format, _args...));
        }

        template<const uint32_t _Out = 0, typename... _Args>
        static void Error(const std::string& _format, _Args... _args) {
            Error(Utils::StringFormat(_format, _args...));
        }

        template<const uint32_t _Out = 0, typename... _Args>
        static void Warning(const std::string& _format, _Args... _args) {
            Warning(Utils::StringFormat(_format, _args...));
        }

        static void Info(const std::string& _str);

        static void Error(const std::string& _str);

        static void Warning(const std::string& _str);

        static void OpenLogFile(const std::filesystem::path& _path);

        static void CloseLogFile();

        static void Flush() {
            if (sOutputMode & OutputMode::Console) {
                std::cout.flush();
                std::cerr.flush();
            }

            if (sOutputMode & OutputMode::File && sFileStream.is_open())
                sFileStream.flush();
        }

    private:
        static std::mutex sMutex;
        static std::ofstream sFileStream;

        static Flags<OutputMode> sOutputMode;
    };
}

#ifndef MX_LOG_LEVEL_INFO
#define MX_LOG_LEVEL_INFO 0
#endif // MX_LOG_LEVEL_INFO

#ifndef MX_LOG_LEVEL_WARNING
#define MX_LOG_LEVEL_WARNING 1
#endif // MX_LOG_LEVEL_WARNING

#ifndef MX_LOG_LEVEL_ERROR
#define MX_LOG_LEVEL_ERROR 2
#endif // MX_LOG_LEVEL_ERROR

#ifndef MX_LOG_LEVEL
#   if MX_DEBUG_MODE
#       define MX_LOG_LEVEL MX_LOG_LEVEL_INFO
#   else
#       define MX_LOG_LEVEL MX_LOG_LEVEL_WARNING
#endif // MX_DEBUG_MODE
#endif // MX_LOG_LEVEL

#ifndef MX_LOG
#define MX_LOG(level,format, ...) \
    {\
        if(level>=MX_LOG_LEVEL){ \
            Log::Info(Utils::StringFormat(format, ##__VA_ARGS__)+std::string("\n -")+ \
                  __FUNCTION__+"\n -["+__FILE__+":"+std::to_string(__LINE__)+"]\n"); \
        }\
    }
#endif // MX_LOG

#ifndef MX_LOG_INFO
#define MX_LOG_INFO(format, ...)\
    MX_LOG(MX_LOG_LEVEL_INFO, std::string("Info: ") + format, ##__VA_ARGS__)
#endif // MX_LOG_INFO


#ifndef MX_LOG_WARNING
#define MX_LOG_WARNING(format, ...)\
        MX_LOG(MX_LOG_LEVEL_WARNING, std::string("Warning: ") + format, ##__VA_ARGS__)
#endif // MX_LOG_WARNING

#ifndef MX_LOG_ERROR
#define MX_LOG_ERROR(format, ...)\
        MX_LOG(MX_LOG_LEVEL_ERROR, std::string("Error: ") + format, ##__VA_ARGS__)
#endif // MX_LOG_ERROR

#endif // MX_LOG_H_
