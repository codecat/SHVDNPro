#pragma once

#include <Script.h>

namespace GTA
{
	public ref class ScriptDomain : public System::MarshalByRefObject
	{
	internal:
		array<System::Type^>^ m_types;
		array<GTA::Script^>^ m_scripts;

	public:
		ScriptDomain();

	public:
		void FindAllTypes();
		bool ScriptInit(int scriptIndex, void* fiberMain, void* fiberScript);

		void OnUnhandledException(System::Object^ sender, System::UnhandledExceptionEventArgs^ e);
		System::Reflection::Assembly^ OnAssemblyResolve(System::Object^ sender, System::ResolveEventArgs^ args);
	};
}
