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

            template<const uint32_t _Out = 0, typename... _Args>
            static void Info(const std::string& _format, _Args... _args) {
                Info(_Out, Utils::StringFormat(_format, _args...));
            }

            template<const uint32_t _Out = 0, typename... _Args>
            static void Error(const std::string& _format, _Args... _args) {
                Error(_Out, Utils::StringFormat(_format, _args...));
            }

            template<const uint32_t _Out = 0, typename... _Args>
            static void Warning(const std::string& _format, _Args... _args) {
                Warning(_Out, Utils::StringFormat(_format, _args...));
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
			static void Info(const uint32_t _out, const std::string& _str);

			static void Error(const uint32_t _out, const std::string& _str);

			static void Warning(const uint32_t _out, const std::string& _str);

            static std::mutex sMutex;
            static std::ofstream sFileStream;

            static OutputMode sOutputMode;
        };
    }
}

#endif
