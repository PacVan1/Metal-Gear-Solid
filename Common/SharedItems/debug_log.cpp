#include "common.h"
#include "debug_log.h"

#define LOG_BLACK	"\033[30m"
#define LOG_RED		"\033[31m"
#define LOG_GREEN	"\033[32m"
#define LOG_YELLOW	"\033[33m"
#define LOG_BLUE	"\033[34m"
#define LOG_MAGENTA "\033[35m"
#define LOG_CYAN	"\033[36m"
#define LOG_WHITE	"\033[37m"
#define LOG_BACK	"\033[0m"

void _DebugLog(u8 const type, char const* message, char const* file, int const line)
{
	namespace fs = std::filesystem;

	switch (type)
	{
	case INFO:
	{
		cout << LOG_WHITE << "[INFO]   \t[" << message << "]" << LOG_BACK << endl;
		break;
	}
	case WARNING:
	{
		cout << LOG_YELLOW << "[WARNING]\t[" << message << "]" << LOG_BACK << endl;
		cout << LOG_YELLOW << "at: " << file << " " << line << LOG_BACK << endl;
		break;
	}
	case ERROR: 
	{
		cout << LOG_RED << "[ERROR]  \t[" << message << "]" << LOG_BACK << endl;
		cout << LOG_RED << "at: " << file << " " << line << LOG_BACK << endl;
		break;
	}
	case ASSET_SAVED:
	{
		cout << LOG_GREEN << "[SAVED]  \t[" << message << "]" << LOG_BACK << endl;
		break;
	}
	case ASSET_LOADED:
	{
		cout << LOG_GREEN << "[LOADED] \t[" << message << "]" << LOG_BACK << endl;
		break;
	}
	case ASSET_FAILED:
	{
		// assuming message is a relative file path
		fs::path full = 
			fs::exists(message) ? 
			fs::canonical(message) : 
			fs::absolute(message);
		cout << LOG_YELLOW << "[FAILED] \t[" << full << "]" << LOG_BACK << endl;
		cout << LOG_YELLOW << "at: " << file << " " << line << LOG_BACK << endl;
		break;
	}
	case ASSET_GENERATED:
	{
		// assuming message is a relative file path
		cout << LOG_BLUE << "[GENERATED]\t[" << message << "]" << LOG_BACK << endl;
		break;
	}
	}
}
