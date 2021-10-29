#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

/* Logs by default enabled */
bool Log::s_Enabled = true;

std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::SetEnabled( bool enabled ) {
	s_Enabled = enabled;
}
bool Log::IsEnabled() {
	return s_Enabled;
}
void Log::Init() {
	//spdlog::set_pattern( "%^[%T] tid:%t pid:%P [%l] %v%$" );
	//s_Logger = spdlog::stdout_color_mt( "Main" );
	spdlog::set_pattern( "%^[%T] [%l] %v%$" );
	s_Logger = spdlog::stdout_color_st( "Main" );
	s_Logger->set_level( spdlog::level::trace );
}