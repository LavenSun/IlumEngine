#pragma once

#define SPDLOG_WCHAR_FILENAMES

#include <spdlog/spdlog.h>

#include <deque>

namespace Ilum
{
class Logger
{
  public:
	static Logger &getInstance();

	~Logger();

	// Thread unsafe
	const std::deque<std::string> getLogs(const std::string &name);

	// Thread safe
	const std::vector<std::string> copyLogs(const std::string &name);

	void clear(const std::string &name);

	void clear();

	void save(const std::string &name);

	void save();

	template <typename FormatString, typename... Args>
	void log(const std::string &name, spdlog::level::level_enum lvl, const FormatString &fmt, const Args... args)
	{
		if (!valid(name))
		{
			return;
		}
		auto &logger = m_loggers.at(name);
		logger->log(lvl, fmt, args...);
	}

	template <typename FormatString, typename... Args>
	void debug(const std::string &name, bool x, const FormatString &fmt, const Args... args)
	{
		if (!valid(name))
		{
			return;
		}
		if (!x)
		{
			auto &logger = m_loggers.at(name);
			logger->log(spdlog::level::err, fmt, args...);
#ifdef _DEBUG
			__debugbreak();
#endif        // _DEBUG
		}
	}

  private:
	bool valid(const std::string &name);

	std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> m_loggers;
};

#define LOG_INFO(...) Logger::getInstance().log("logger", spdlog::level::info, __VA_ARGS__);
#define LOG_WARN(...) Logger::getInstance().log("logger", spdlog::level::warn, __VA_ARGS__);
#define LOG_ERROR(...) Logger::getInstance().log("logger", spdlog::level::err, __VA_ARGS__);
#define LOG_TRACE(...) Logger::getInstance().log("logger", spdlog::level::trace, __VA_ARGS__);
#define LOG_CRITICAL(...) Logger::getInstance().log("logger", spdlog::level::critical, __VA_ARGS__);
#define LOG_DEBUG(x, ...) Logger::getInstance().debug("logger", x, __VA_ARGS__);

#define VK_INFO(...) Logger::getInstance().log("vulkan", spdlog::level::info, __VA_ARGS__);
#define VK_WARN(...) Logger::getInstance().log("vulkan", spdlog::level::warn, __VA_ARGS__);
#define VK_ERROR(...) Logger::getInstance().log("vulkan", spdlog::level::err, __VA_ARGS__);
#define VK_TRACE(...) Logger::getInstance().log("vulkan", spdlog::level::trace, __VA_ARGS__);
#define VK_CRITICAL(...) Logger::getInstance().log("vulkan", spdlog::level::critical, __VA_ARGS__);
#define VK_DEBUG(x, ...) Logger::getInstance().debug("vulkan", x, __VA_ARGS__);
}        // namespace Ilum