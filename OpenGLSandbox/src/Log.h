#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

class Log {
public:
	Log( Log const& ) = delete;
	void operator=( const Log& ) = delete;

	/* Should be called in main */
	static void Init();

	static void SetEnabled( bool enabled = true );
	static bool IsEnabled();

	inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }
private:
	Log() {}

private:
	/* spdlog stdout colored singlethreaded/multithreaded Logger */
	static std::shared_ptr<spdlog::logger> s_Logger;\

	static bool s_Enabled;
};

#define LOG_TRACE( ... )	if( Log::IsEnabled() ) Log::GetLogger()->trace( __VA_ARGS__ )
#define LOG_WARN( ... )		if( Log::IsEnabled() ) Log::GetLogger()->warn( __VA_ARGS__ )
#define LOG_INFO( ... )		if( Log::IsEnabled() ) Log::GetLogger()->info( __VA_ARGS__ )
#define LOG_ERROR( ... )	if( Log::IsEnabled() ) Log::GetLogger()->error( __VA_ARGS__ )
#define LOG_FATAL( ... )	if( Log::IsEnabled() ) Log::GetLogger()->fatal( __VA_ARGS__ )