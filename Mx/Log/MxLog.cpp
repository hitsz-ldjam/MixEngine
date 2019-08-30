#include "MxLog.h"

namespace Mix {
	std::ofstream Log::sFileStream;
	std::mutex Log::sMutex;
	Flags<OutputMode> Log::sOutputMode = OutputMode::Console;

	void Log::Info(const std::string& _str) {
		std::lock_guard<std::mutex> lock(sMutex);
		if (sOutputMode.isSet(OutputMode::Console))
			std::cout << Utils::StringFormat("%s", _str.c_str()) << std::endl;

		if (sOutputMode.isSet(OutputMode::File)) {
			if (sFileStream.is_open())
				sFileStream << Utils::StringFormat("%s", _str.c_str()) << std::endl;
			else {
				std::cerr << Utils::StringFormat("Warning : %s: The output mode [ FILE ] bit has been specified, "
												 "but the output file is not specified", __FUNCTION__) << std::endl;
				sOutputMode.unset(OutputMode::File);
			}

		}
	}

	void Log::Error(const std::string& _str) {
		std::lock_guard<std::mutex> lock(sMutex);
		if (sOutputMode.isSet(OutputMode::Console))
			std::cerr << Utils::StringFormat("%s", _str.c_str()) << std::endl;

		if (sOutputMode.isSet(OutputMode::File)) {
			if (sFileStream.is_open())
				sFileStream << Utils::StringFormat("%s", _str.c_str()) << std::endl << std::flush;
			else {
				std::cerr << Utils::StringFormat("Warning : %s: The output mode [ FILE ] bit has been specified, "
												 "but the output file is not specified", __FUNCTION__) << std::endl;
				sOutputMode.unset(OutputMode::File);
			}
		}
	}

	void Log::Warning(const std::string& _str) {
		std::lock_guard<std::mutex> lock(sMutex);
		if (sOutputMode.isSet(OutputMode::Console))
			std::cout << Utils::StringFormat("%s", _str.c_str()) << std::endl;

		if (sOutputMode.isSet(OutputMode::File)) {
			if (sFileStream.is_open())
				sFileStream << Utils::StringFormat("%s", _str.c_str()) << std::endl;
			else {
				std::cerr << Utils::StringFormat("Warning : %s: The output mode [ FILE ] bit has been specified, "
												 "but the output file is not specified", __FUNCTION__) << std::endl;
				sOutputMode.unset(OutputMode::File);
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

		sOutputMode |= OutputMode::File;
		sFileStream << Utils::StringFormat("-------%s--%s--------", __DATE__, __TIME__) << std::endl << std::flush;
	}

	void Log::CloseLogFile() {
		if (sFileStream.is_open())
			sFileStream.close();
	}
}
