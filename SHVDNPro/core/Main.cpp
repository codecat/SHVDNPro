#include <vector>

#include <Script.h>
#include <Function.h>
#include <Input.h>

#include <MemoryAccess.h>
#include <NativeObjects.h>
#include <NativeHashes.h>
#include <INativeValue.h>
#include <OutputArgument.h>

#include <Matrix.hpp>
#include <Quaternion.hpp>
#include <Vector2.hpp>
#include <Vector3.hpp>

#include <ManagedGlobals.h>

ref class ManagedEventSink
{
public:
	System::Reflection::Assembly^ OnAssemblyResolve(System::Object^ sender, System::ResolveEventArgs^ args)
	{
		auto exeAssembly = System::Reflection::Assembly::GetExecutingAssembly();
		if (args->Name == exeAssembly->FullName) {
			return exeAssembly;
		}

		return nullptr;
	}

	void OnUnhandledException(System::Object^ sender, System::UnhandledExceptionEventArgs^ args)
	{
		GTA::ManagedGlobals::g_logWriter->WriteLine("*** Unhandled exception: {0}", args->ExceptionObject->ToString());
	}
};

array<GTA::Script^>^ LoadAllScripts()
{
	auto ret = gcnew System::Collections::Generic::List<GTA::Script^>();

	auto curDir = System::Reflection::Assembly::GetExecutingAssembly()->Location;
	curDir = System::IO::Path::GetDirectoryName(curDir);
	auto files = System::IO::Directory::GetFiles(curDir + "\\Scripts\\", "*.dll", System::IO::SearchOption::AllDirectories);

	//TODO: Load using AppDomain so we can unload (and shadowcopy) properly

	for each (auto file in files) {
		auto fileName = System::IO::Path::GetFileName(file);

		System::Reflection::Assembly^ assembly = nullptr;
		try {
			assembly = System::Reflection::Assembly::LoadFrom(file);
		} catch (System::Exception^ ex) {
			GTA::ManagedGlobals::g_logWriter->WriteLine("*** Assembly load exception for {0}: {1}", fileName, ex->ToString());
		}

		GTA::ManagedGlobals::g_logWriter->WriteLine("Loaded assembly {0}", assembly);

		try {
			for each (auto type in assembly->GetTypes()) {
				if (!type->IsSubclassOf(GTA::Script::typeid)) {
					continue;
				}

				GTA::Script^ script = static_cast<GTA::Script^>(System::Activator::CreateInstance(type));
				ret->Add(script);

				GTA::ManagedGlobals::g_logWriter->WriteLine("Instantiated {0}", type->FullName);
			}
		} catch (System::Reflection::ReflectionTypeLoadException^ ex) {
			GTA::ManagedGlobals::g_logWriter->WriteLine("*** Exception while loading type: {0}", ex->ToString());
			for each (auto loaderEx in ex->LoaderExceptions) {
				GTA::ManagedGlobals::g_logWriter->WriteLine("***    {0}", loaderEx->ToString());
			}
			continue;
		} catch (System::Exception^ ex) {
			GTA::ManagedGlobals::g_logWriter->WriteLine("*** Exception while loading assembly: {0}", ex->ToString());
			continue;
		}
	}

	return ret->ToArray();
}

static void ManagedScriptInit(int scriptIndex, void* fiberMain, void* fiberScript)
{
	auto script = GTA::ManagedGlobals::g_scripts[scriptIndex];
	script->m_fiberMain = fiberMain;
	script->m_fiberCurrent = fiberScript;
}

static int ManagedScriptGetWaitTime(int scriptIndex)
{
	return GTA::ManagedGlobals::g_scripts[scriptIndex]->m_fiberWait;
}

static void ManagedScriptTick(int scriptIndex)
{
	try {
		GTA::ManagedGlobals::g_scripts[scriptIndex]->OnTick();
	} catch (System::Exception^ ex) {
		GTA::ManagedGlobals::g_logWriter->WriteLine("*** Exception during OnTick: {0}", ex->ToString());
	}
	GTA::Native::Function::ClearStringPool();
}

static void ManagedInitialize();

#pragma unmanaged
#include <main.h>
#include <Windows.h>
#include <cstdarg>


static void UnmanagedLogWrite(const char* format, ...)
{
	FILE* fh = fopen("SHVDNProUnmanaged.log", "ab");
	if (fh == nullptr) {
		return;
	}

	va_list va;
	va_start(va, format);
	vfprintf(fh, format, va);
	va_end(va);

	fclose(fh);
}


struct ScriptFiberInfo
{
	int m_index;
	void* m_fiberMain;
	void* m_fiberScript;
};

static int _currentIndex = 0;

static HMODULE _instance;

static std::vector<ScriptFiberInfo> _scriptFibers;

static void ScriptMainFiber(LPVOID pv)
{
	while (true) {
		ScriptFiberInfo* fi = (ScriptFiberInfo*)pv;
		ManagedScriptTick(fi->m_index);
		SwitchToFiber(fi->m_fiberMain);
	}
}

static void ScriptMain(int index)
{
	ScriptFiberInfo fi;
	//fi.m_index = _currentIndex++;
	fi.m_index = index;
	fi.m_fiberMain = GetCurrentFiber();
	fi.m_fiberScript = CreateFiber(0, ScriptMainFiber, (LPVOID)&fi);

	ManagedScriptInit(fi.m_index, fi.m_fiberMain, fi.m_fiberScript);

	while (true) {
		int ms = ManagedScriptGetWaitTime(fi.m_index);
		scriptWait(ms);
		SwitchToFiber(fi.m_fiberScript);
	}
}

// uh
// yeah
// it would be nice if shv gave us some more leeway here
#pragma optimize("", off)
static void ScriptMain_Wrapper0() { ScriptMain(0); }
static void ScriptMain_Wrapper1() { ScriptMain(1); }
static void ScriptMain_Wrapper2() { ScriptMain(2); }
static void ScriptMain_Wrapper3() { ScriptMain(3); }
static void ScriptMain_Wrapper4() { ScriptMain(4); }
static void ScriptMain_Wrapper5() { ScriptMain(5); }
static void ScriptMain_Wrapper6() { ScriptMain(6); }
static void ScriptMain_Wrapper7() { ScriptMain(7); }
static void ScriptMain_Wrapper8() { ScriptMain(8); }
static void ScriptMain_Wrapper9() { ScriptMain(9); }
static void ScriptMain_Wrapper10() { ScriptMain(10); }
static void ScriptMain_Wrapper11() { ScriptMain(11); }
static void ScriptMain_Wrapper12() { ScriptMain(12); }
static void ScriptMain_Wrapper13() { ScriptMain(13); }
static void ScriptMain_Wrapper14() { ScriptMain(14); }
static void ScriptMain_Wrapper15() { ScriptMain(15); }
static void ScriptMain_Wrapper16() { ScriptMain(16); }
static void ScriptMain_Wrapper17() { ScriptMain(17); }
static void ScriptMain_Wrapper18() { ScriptMain(18); }
static void ScriptMain_Wrapper19() { ScriptMain(19); }
static void ScriptMain_Wrapper20() { ScriptMain(20); }
#pragma optimize("", on)

static void(*_scriptWrappers[])() = {
	&ScriptMain_Wrapper0,
	&ScriptMain_Wrapper1,
	&ScriptMain_Wrapper2,
	&ScriptMain_Wrapper3,
	&ScriptMain_Wrapper4,
	&ScriptMain_Wrapper5,
	&ScriptMain_Wrapper6,
	&ScriptMain_Wrapper7,
	&ScriptMain_Wrapper8,
	&ScriptMain_Wrapper9,
	&ScriptMain_Wrapper10,
	&ScriptMain_Wrapper11,
	&ScriptMain_Wrapper12,
	&ScriptMain_Wrapper13,
	&ScriptMain_Wrapper14,
	&ScriptMain_Wrapper15,
	&ScriptMain_Wrapper16,
	&ScriptMain_Wrapper17,
	&ScriptMain_Wrapper18,
	&ScriptMain_Wrapper19,
	&ScriptMain_Wrapper20,
};

static void RegisterScriptMain(int index)
{
	if (index > 20) {
		//TODO: Log some error?
		return;
	}
	UnmanagedLogWrite("RegisterScriptMain(%d) : %p\n", index, _scriptWrappers[index]);
	scriptRegister(_instance, _scriptWrappers[index]);
}

static void ScriptKeyboardMessage(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow)
{
	ManagedScriptKeyboardMessage(key, repeats, scanCode, isExtended, isWithAlt, wasDownBefore, isUpNow);
}

static void DXGIPresent(void* swapChain)
{
	//TODO
}
#pragma managed

static void ManagedInitialize()
{
	GTA::ManagedGlobals::g_logWriter = gcnew System::IO::StreamWriter("SHVDNPro.log", true);
	GTA::ManagedGlobals::g_logWriter->AutoFlush = true;
	GTA::ManagedGlobals::g_logWriter->WriteLine("SHVDN Pro Initializing");

	auto eventSink = gcnew ManagedEventSink();
	System::AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(eventSink, &ManagedEventSink::OnAssemblyResolve);
	System::AppDomain::CurrentDomain->UnhandledException += gcnew System::UnhandledExceptionEventHandler(eventSink, &ManagedEventSink::OnUnhandledException);

	GTA::ManagedGlobals::g_scripts = LoadAllScripts();
	GTA::ManagedGlobals::g_logWriter->WriteLine("{0} scripts instantiated", GTA::ManagedGlobals::g_scripts->Length);

	for (int i = 0; i < GTA::ManagedGlobals::g_scripts->Length; i++) {
		RegisterScriptMain(i);
	}
}

#pragma unmanaged
BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hInstance);
		_instance = hInstance;

		ManagedInitialize();
		keyboardHandlerRegister(&ScriptKeyboardMessage);
		presentCallbackRegister(&DXGIPresent);
	} else if (reason == DLL_PROCESS_DETACH) {
		for (auto fi : _scriptFibers) {
			DeleteFiber(fi.m_fiberScript);
		}
		scriptUnregister(hInstance);
	}
	return TRUE;
}
