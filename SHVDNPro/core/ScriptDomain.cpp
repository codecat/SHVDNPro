#include <ScriptDomain.h>

#include <ManagedGlobals.h>

GTA::ScriptDomain::ScriptDomain()
{
	GTA::ManagedGlobals::g_logWriter->WriteLine("ScriptDomain created.");
}

void GTA::ScriptDomain::FindAllTypes()
{
	auto ret = gcnew System::Collections::Generic::List<System::Type^>();

	auto curDir = System::Reflection::Assembly::GetExecutingAssembly()->Location;
	curDir = System::IO::Path::GetDirectoryName(curDir);

	auto files = System::IO::Directory::GetFiles(curDir + "\\Scripts\\", "*.dll", System::IO::SearchOption::AllDirectories);

	for each (auto file in files) {
		GTA::ManagedGlobals::g_logWriter->WriteLine("Loading assembly {0}", file);

		auto fileName = System::IO::Path::GetFileName(file);

		System::Reflection::Assembly^ assembly = nullptr;
		try {
			assembly = System::Reflection::Assembly::LoadFrom(file);
		} catch (System::BadImageFormatException^) {
			continue;
		} catch (System::Exception^ ex) {
			GTA::ManagedGlobals::g_logWriter->WriteLine("*** Assembly load exception for {0}: {1}", fileName, ex->ToString());
		} catch (...) {
			GTA::ManagedGlobals::g_logWriter->WriteLine("*** Unmanaged exception while loading assembly!");
		}

		GTA::ManagedGlobals::g_logWriter->WriteLine("Loaded assembly {0}", assembly);

		try {
			for each (auto type in assembly->GetTypes()) {
				if (!type->IsSubclassOf(GTA::Script::typeid)) {
					continue;
				}

				ret->Add(type);

				GTA::ManagedGlobals::g_logWriter->WriteLine("Registered type {0}", type->FullName);
			}
		} catch (System::Reflection::ReflectionTypeLoadException^ ex) {
			GTA::ManagedGlobals::g_logWriter->WriteLine("*** Exception while iterating types: {0}", ex->ToString());
			for each (auto loaderEx in ex->LoaderExceptions) {
				GTA::ManagedGlobals::g_logWriter->WriteLine("***    {0}", loaderEx->ToString());
			}
			continue;
		} catch (System::Exception^ ex) {
			GTA::ManagedGlobals::g_logWriter->WriteLine("*** Exception while iterating types: {0}", ex->ToString());
			continue;
		} catch (...) {
			GTA::ManagedGlobals::g_logWriter->WriteLine("*** Unmanaged exception while iterating types");
			continue;
		}

		GTA::ManagedGlobals::g_logWriter->WriteLine("Done iterating assembly");
	}

	if (ret->Count > 20) {
		GTA::ManagedGlobals::g_logWriter->WriteLine("*** WARNING: We can't have more than 20 scripts, yet we have {0}!", ret->Count);
	}

	m_types = ret->ToArray();
	m_scripts = gcnew array<GTA::Script^>(m_types->Length);
}

bool GTA::ScriptDomain::ScriptInit(int scriptIndex, void* fiberMain, void* fiberScript)
{
	auto scriptType = m_types[scriptIndex];

	GTA::ManagedGlobals::g_logWriter->WriteLine("Instantiating {0}", scriptType->FullName);

	GTA::Script^ script = nullptr;
	try {
		script = static_cast<GTA::Script^>(System::Activator::CreateInstance(scriptType));
	} catch (System::Reflection::ReflectionTypeLoadException^ ex) {
		GTA::ManagedGlobals::g_logWriter->WriteLine("*** Exception while instantiating script: {0}", ex->ToString());
		for each (auto loaderEx in ex->LoaderExceptions) {
			GTA::ManagedGlobals::g_logWriter->WriteLine("***    {0}", loaderEx->ToString());
		}
		return false;
	} catch (System::Exception^ ex) {
		GTA::ManagedGlobals::g_logWriter->WriteLine("*** Exception while instantiating script: {0}", ex->ToString());
		return false;
	} catch (...) {
		GTA::ManagedGlobals::g_logWriter->WriteLine("*** Unmanaged exception while instantiating script!");
		return false;
	}

	GTA::ManagedGlobals::g_logWriter->WriteLine("Instantiated {0}", scriptType->FullName);

	m_scripts[scriptIndex] = script;
	script->m_fiberMain = fiberMain;
	script->m_fiberCurrent = fiberScript;
	script->OnInit();

	return true;
}
