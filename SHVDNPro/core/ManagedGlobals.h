#pragma once

#include <Script.h>
#include <ScriptDomain.h>

namespace GTA
{
	public ref class ManagedGlobals
	{
	public:
		static System::IO::StreamWriter^ g_logWriter;
		static System::AppDomain^ g_appDomain;
		static GTA::ScriptDomain^ g_scriptDomain;
	};
}
