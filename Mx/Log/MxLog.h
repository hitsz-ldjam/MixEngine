#pragma once
#ifndef MX_LOG_H_
#define MX_LOG_H_
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <mutex>

namespace Mix {
    namespace Debug {
        template<typename... _Args>
        static std::string StringFormat(const std::string &_format, _Args &&... _args) {
            const size_t size = snprintf(nullptr, 0, _format.c_str(), _args ...) + 1; // Extra space for '\0'
            std::unique_ptr<char[]> buf(new char[size]);
            snprintf(buf.get(), size, _format.c_str(), _args ...);
            return std::string(buf.get(), buf.get() + size - 1); // Excludes the '\0'
        }

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
                Info(StringFormat(_format, _args...), _Out);
            }

            static void Error(const std::string& _str, const uint32_t _out = 0);

            template<const uint32_t _Out = 0, typename... _Args>
            static void Error(const std::string& _format, _Args... _args) {
                Error(StringFormat(_format, _args...), _Out);
            }

            static void Warning(const std::string& _str, const uint32_t _out = 0);

            template<const uint32_t _Out = 0, typename... _Args>
            static void Warning(const std::string& _format, _Args... _args) {
                Warning(StringFormat(_format, _args...), _Out);
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
