#include <ScriptDomain.h>

#include <Function.h>

#include <ManagedGlobals.h>
#include <Log.h>

#pragma unmanaged
#include <Windows.h>
#pragma managed

GTA::ScriptDomain::ScriptDomain()
{
	System::AppDomain::CurrentDomain->UnhandledException += gcnew System::UnhandledExceptionEventHandler(this, &GTA::ScriptDomain::OnUnhandledException);
	System::AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(this, &GTA::ScriptDomain::OnAssemblyResolve);

	GTA::ManagedGlobals::g_scriptDomain = this;
}

void GTA::ScriptDomain::FindAllTypes()
{
	auto ret = gcnew System::Collections::Generic::List<System::Type^>();

	auto curDir = System::Reflection::Assembly::GetExecutingAssembly()->Location;
	curDir = System::IO::Path::GetDirectoryName(curDir);

	auto files = System::IO::Directory::GetFiles(curDir + "\\Scripts\\", "*.dll", System::IO::SearchOption::AllDirectories);

	for each (auto file in files) {
		auto fileName = System::IO::Path::GetFileName(file);

		System::Reflection::Assembly^ assembly = nullptr;
		try {
			assembly = System::Reflection::Assembly::LoadFrom(file);
		} catch (System::BadImageFormatException^) {
			continue;
		} catch (System::Exception^ ex) {
			GTA::WriteLog("*** Assembly load exception for {0}: {1}", fileName, ex->ToString());
		} catch (...) {
			GTA::WriteLog("*** Unmanaged exception while loading assembly!");
		}

		GTA::WriteLog("Loaded assembly {0}", assembly);

		try {
			for each (auto type in assembly->GetTypes()) {
				if (!type->IsSubclassOf(GTA::Script::typeid)) {
					continue;
				}

				ret->Add(type);

				GTA::WriteLog("Registered type {0}", type->FullName);
			}
		} catch (System::Reflection::ReflectionTypeLoadException^ ex) {
			GTA::WriteLog("*** Exception while iterating types: {0}", ex->ToString());
			for each (auto loaderEx in ex->LoaderExceptions) {
				GTA::WriteLog("***    {0}", loaderEx->ToString());
			}
			continue;
		} catch (System::Exception^ ex) {
			GTA::WriteLog("*** Exception while iterating types: {0}", ex->ToString());
			continue;
		} catch (...) {
			GTA::WriteLog("*** Unmanaged exception while iterating types");
			continue;
		}
	}

	if (ret->Count > 20) {
		GTA::WriteLog("*** WARNING: We can't have more than 20 scripts, yet we have {0}!", ret->Count);
	}

	m_types = ret->ToArray();
	m_scripts = gcnew array<GTA::Script^>(m_types->Length);

	GTA::WriteLog("{0} script types found:", m_types->Length);
	for (int i = 0; i < m_types->Length; i++) {
		GTA::WriteLog("  {0}: {1}", i, m_types[i]->FullName);
	}
}

GTA::Script^ GTA::ScriptDomain::GetExecuting()
{
	void* currentFiber = GetCurrentFiber();

	// I don't know if GetCurrentFiber ever returns null, but whatever
	if (currentFiber == nullptr) {
		return nullptr;
	}

	for each (auto script in m_scripts) {
		if (script != nullptr && script->m_fiberCurrent == currentFiber) {
			return script;
		}
	}

	return nullptr;
}

bool GTA::ScriptDomain::ScriptInit(int scriptIndex, void* fiberMain, void* fiberScript)
{
	auto scriptType = m_types[scriptIndex];

	GTA::WriteLog("Instantiating {0}", scriptType->FullName);

	GTA::Script^ script = nullptr;
	try {
		script = static_cast<GTA::Script^>(System::Activator::CreateInstance(scriptType));
	} catch (System::Reflection::ReflectionTypeLoadException^ ex) {
		GTA::WriteLog("*** Exception while instantiating script: {0}", ex->ToString());
		for each (auto loaderEx in ex->LoaderExceptions) {
			GTA::WriteLog("***    {0}", loaderEx->ToString());
		}
		return false;
	} catch (System::Exception^ ex) {
		GTA::WriteLog("*** Exception while instantiating script: {0}", ex->ToString());
		return false;
	} catch (...) {
		GTA::WriteLog("*** Unmanaged exception while instantiating script!");
		return false;
	}

	GTA::WriteLog("Instantiated {0}", scriptType->FullName);

	m_scripts[scriptIndex] = script;
	script->m_fiberMain = fiberMain;
	script->m_fiberCurrent = fiberScript;

	try {
		script->OnInit();
	} catch (System::Exception^ ex) {
		GTA::WriteLog("*** Exception in script OnInit: {0}", ex->ToString());
		return false;
	} catch (...) {
		GTA::WriteLog("*** Unmanaged exception in script OnInit!");
		return false;
	}

	return true;
}

bool GTA::ScriptDomain::ScriptExists(int scriptIndex)
{
	return scriptIndex < m_types->Length;
}

int GTA::ScriptDomain::ScriptGetWaitTime(int scriptIndex)
{
	auto script = m_scripts[scriptIndex];
	if (script == nullptr) {
		return 0;
	}
	return script->m_fiberWait;
}

void GTA::ScriptDomain::ScriptResetWaitTime(int scriptIndex)
{
	auto script = m_scripts[scriptIndex];
	if (script == nullptr) {
		return;
	}
	script->m_fiberWait = 0;
}

void GTA::ScriptDomain::ScriptTick(int scriptIndex)
{
	try {
		auto script = m_scripts[scriptIndex];
		if (script != nullptr) {
			script->ProcessOneTick();
		}
	} catch (System::Exception^ ex) {
		GTA::WriteLog("*** Exception in script ProcessOneTick: {0}", ex->ToString());
	} catch (...) {
		GTA::WriteLog("*** Unmanaged exception in script ProcessOneTick!");
	}

	try {
		GTA::Native::Function::ClearStringPool();
	} catch (System::Exception^ ex) {
		GTA::WriteLog("*** Exception while clearing string pool: {0}", ex->ToString());
	} catch (...) {
		GTA::WriteLog("*** Unmanaged exception while clearning string pool!");
	}
}

void GTA::ScriptDomain::QueueKeyboardEvent(System::Tuple<bool, System::Windows::Forms::Keys>^ ev)
{
	for each (auto script in m_scripts) {
		if (script == nullptr) {
			continue;
		}
		script->m_keyboardEvents->Enqueue(ev);
	}
}

void GTA::ScriptDomain::OnUnhandledException(System::Object^ sender, System::UnhandledExceptionEventArgs^ e)
{
	GTA::WriteLog("*** Unhandled exception: {0}", e->ExceptionObject->ToString());
}

System::Reflection::Assembly^ GTA::ScriptDomain::OnAssemblyResolve(System::Object^ sender, System::ResolveEventArgs^ args)
{
	if (args->RequestingAssembly != nullptr) {
		GTA::WriteLog("Resolving assembly: \"{0}\" from: \"{1}\"", args->Name, args->RequestingAssembly->FullName);
	} else {
		GTA::WriteLog("Resolving assembly: \"{0}\"", args->Name);
	}

	auto exeAssembly = System::Reflection::Assembly::GetExecutingAssembly();
	if (args->Name == exeAssembly->FullName) {
		GTA::WriteLog("  Returning exeAssembly: \"{0}\"", exeAssembly->FullName);
		return exeAssembly;
	}

	GTA::WriteLog("  Returning nullptr");
	return nullptr;
}
