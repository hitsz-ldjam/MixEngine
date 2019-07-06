#include "MxLog.h"

namespace Mix {
	namespace Debug {
		std::ofstream Log::sFileStream;
		std::mutex Log::sMutex;
		Log::OutputMode Log::sOutputMode = CONSOLE;

		void Log::Info(const uint32_t _out, const std::string& _str) {
			const auto mode = _out != 0 ? static_cast<OutputMode>(_out) : sOutputMode;
			std::lock_guard<std::mutex> lock(sMutex);
			if (mode & CONSOLE)
				std::cout << Utils::StringFormat("%s", _str.c_str()) << std::endl;

			if (mode & FILE) {
				if (sFileStream.is_open())
					sFileStream << Utils::StringFormat("%s", _str.c_str()) << std::endl;
				else {
					std::cerr << Utils::StringFormat("Warning : %s: The output mode [ FILE ] bit has been specified, "
													 "but the output file is not specified", __FUNCTION__) << std::endl;
					sOutputMode =static_cast<OutputMode>(sOutputMode & ~FILE);
				}
					
			}
		}

		void Log::Error(const uint32_t _out, const std::string& _str) {
			const auto mode = _out != 0 ? static_cast<OutputMode>(_out) : sOutputMode;
			std::lock_guard<std::mutex> lock(sMutex);
			if (mode & CONSOLE)
				std::cerr << Utils::StringFormat("%s", _str.c_str()) << std::endl;

			if (mode & FILE) {
				if (sFileStream.is_open())
					sFileStream << Utils::StringFormat("%s", _str.c_str()) << std::endl << std::flush;
				else {
					std::cerr << Utils::StringFormat("Warning : %s: The output mode [ FILE ] bit has been specified, "
													 "but the output file is not specified", __FUNCTION__) << std::endl;
					sOutputMode = static_cast<OutputMode>(sOutputMode & ~FILE);
				}
			}
		}

		void Log::Warning(const uint32_t _out, const std::string& _str) {
			const auto mode = _out != 0 ? static_cast<OutputMode>(_out) : sOutputMode;
			std::lock_guard<std::mutex> lock(sMutex);
			if (mode & CONSOLE)
				std::cout << Utils::StringFormat("%s", _str.c_str()) << std::endl;

			if (mode & FILE) {
				if (sFileStream.is_open())
					sFileStream << Utils::StringFormat("%s", _str.c_str()) << std::endl;
				else {
					std::cerr << Utils::StringFormat("Warning : %s: The output mode [ FILE ] bit has been specified, "
													 "but the output file is not specified", __FUNCTION__) << std::endl;
					sOutputMode = static_cast<OutputMode>(sOutputMode & ~FILE);
				}
					
			}
		}

		void Log::OpenLogFile(const std::filesystem::path& _path) {
			std::lock_guard<std::mutex> lock(sMutex);
			if (sFileStream.is_open()) {
				sFileStream.close();
			}

			sFileStream.open(_path, std::ios_base::app);
			if (!sFileStream.is_open())
				std::cerr << Utils::StringFormat("Warning : %s: Failed to open file %s", __FUNCTION__) << std::endl;

			sOutputMode = static_cast<OutputMode>(sOutputMode | FILE);
			sFileStream << Utils::StringFormat("-------%s--%s--------", __DATE__, __TIME__) << std::endl << std::flush;
		}

		void Log::CloseLogFile() {
			if (sFileStream.is_open())
				sFileStream.close();
		}
	}
}
