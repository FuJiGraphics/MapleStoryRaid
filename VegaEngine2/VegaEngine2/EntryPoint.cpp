#include "pch.h"
#include "EntryPoint.h"

int main(int argc, char** argv)
{
	auto app = fz::CreateSystem();

	app->Init();
	do {
		app->Run();
	} while (app->IsRunning());
	app->Release();
	
	delete app;
	return 0;
}