#include "Logger.h"
#include "windows.h"

namespace Logger
{
	void Log(const std::string & message) {
#ifdef _DEBUG
		OutputDebugStringA(message.c_str());
#endif // _DEBUG
	}
}
