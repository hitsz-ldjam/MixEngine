#pragma once
#ifndef MX_LOG_H_
#define MX_LOG_H_

#include "../Utils/MxUtils.h"

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <mutex>

namespace Mix {
    namespace Debug {
        class Log {
        public:
            enum OutputMode {
                CONSOLE = 0b001,
                FILE = 0b010,
                BOTH = 0b011
            };

            static void SetOutputMode(const OutputMode _out) {
                std::lock_guard<std::mutex> lock(sMutex);
                sOutputMode = _out;
            }

            static OutputMode GetOutputMode() {
                return sOutputMode;
            }

            static void Info(const std::string& _str, const uint32_t _out = 0);

            template<const uint32_t _Out = 0, typename... _Args>
            static void Info(const std::string& _format, _Args... _args) {
                Info(Utils::StringFormat(_format, _args...), _Out);
            }

            static void Error(const std::string& _str, const uint32_t _out = 0);

            template<const uint32_t _Out = 0, typename... _Args>
            static void Error(const std::string& _format, _Args... _args) {
                Error(Utils::StringFormat(_format, _args...), _Out);
            }

            static void Warning(const std::string& _str, const uint32_t _out = 0);

            template<const uint32_t _Out = 0, typename... _Args>
            static void Warning(const std::string& _format, _Args... _args) {
                Warning(Utils::StringFormat(_format, _args...), _Out);
            }

            static void OpenLogFile(const std::filesystem::path& _path);

            static void CloseLogFile();

            static void Flush() {
                if (sOutputMode & CONSOLE) {
                    std::cout.flush();
                    std::cerr.flush();
                }

                if (sOutputMode & FILE && sFileStream.is_open())
                    sFileStream.flush();
            }

        private:
            static std::mutex sMutex;
            static std::ofstream sFileStream;

            static OutputMode sOutputMode;
        };
    }
}

#endif
