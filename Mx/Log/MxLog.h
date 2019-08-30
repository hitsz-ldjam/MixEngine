#pragma once
#ifndef MX_LOG_H_
#define MX_LOG_H_

#include "../Utils/MxUtils.h"
#include "../Utils/MxFlags.h"

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

#endif
