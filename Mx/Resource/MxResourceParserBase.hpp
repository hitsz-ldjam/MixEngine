#pragma once
#ifndef MX_RESOURCE_PARSER_BASE_H_
#define MX_RESOURCE_PARSER_BASE_H_

#include "MxResourceBase.h"

#include <set>
#include <unordered_set>
#include <filesystem>

namespace Mix {
	class ResourceParserBase {
	public:
		ResourceParserBase(const ResourceParserBase& _other) = default;

		ResourceParserBase(ResourceParserBase&& _other) noexcept = default;

		ResourceParserBase& operator=(const ResourceParserBase& _other) = default;

		ResourceParserBase& operator=(ResourceParserBase&& _other) noexcept = default;

		/**
		 * \brief Load file and parse it with specified type
		 */
		virtual std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const ResourceType _type, void* _additionalParam = nullptr) = 0;

		/**
		 * \brief Load file and parse it with specified extension
		 */
		virtual std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const std::string& _ext, void* _additionalParam = nullptr) = 0;

		virtual ~ResourceParserBase() = default;

		/**
		 * \brief Check if ResourceType _type is supported by this parser
		 */
		bool isSupport(const ResourceType _type) const {
			return mSupportedTypes.count(_type);
		}

		/**
		 * \brief Check if extension _ext is supported by this parser
		 */
		bool isSupport(const std::string& _ext) const {
			return mSupportedExts.count(_ext);
		}

		/**
		 * \brief Get all types supported by this parser
		 */
		const std::set<ResourceType>& allSupportedTypes() const {
			return mSupportedTypes;
		}

		/**
		 * \brief Get all extension supported by this parser
		 */
		const std::unordered_set<std::string>& allSupportedExts()const {
			return mSupportedExts;
		}

	protected:
		ResourceParserBase() = default;

		std::set<ResourceType> mSupportedTypes;
		std::unordered_set<std::string> mSupportedExts;

	};
}

#endif
