#include "App.h"

#include "Log.h"

int main()
{
	Log::InitLogTimestamp();
	Log::Init();

	int exitCode = 0;

	try
	{
		App app;
		app.Run();
	}
	catch (const std::exception& err)
	{
		Log::Critical("Fatal error: {}", err.what());
		exitCode = 1;
	}
	catch (...)
	{
		Log::Critical("Unknown fatal exception");
		exitCode = 1;
	}

	Log::Shutdown();
	return exitCode;
}