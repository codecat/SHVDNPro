#pragma once

#include <Script.h>

namespace GTA
{
	public ref class ManagedGlobals
	{
	public:
		static System::IO::StreamWriter^ g_logWriter;
		static array<System::Type^>^ g_scriptTypes;
		static array<GTA::Script^>^ g_scripts;
	};
}
