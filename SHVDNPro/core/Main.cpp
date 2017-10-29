#include <vector>

#include <Script.h>
#include <Function.h>
#include <Input.h>

#include <GlobalVariable.h>
#include <NativeObjects.h>
#include <NativeHashes.h>
#include <INativeValue.h>
#include <OutputArgument.h>

#include <Math/Matrix.hpp>
#include <Math/Quaternion.hpp>
#include <Math/Vector2.hpp>
#include <Math/Vector3.hpp>

#include <ManagedGlobals.h>
#include <ScriptDomain.h>
#include <Log.h>

#include <UnmanagedLog.h>

ref class ManagedEventSink
{
public:
	void OnUnhandledException(System::Object^ sender, System::UnhandledExceptionEventArgs^ args)
	{
		GTA::WriteLog("*** Unhandled exception: {0}", args->ExceptionObject->ToString());
	}
};

void LoadScriptDomain()
{
	auto curDir = System::IO::Path::GetDirectoryName(System::Reflection::Assembly::GetExecutingAssembly()->Location);

	System::String^ strPath = System::Environment::GetEnvironmentVariable("PATH");
	if (!strPath->EndsWith(";")) {
		strPath += ";";
	}
	strPath += curDir + ";";
	strPath += curDir + "\\Scripts;";
	System::Environment::SetEnvironmentVariable("PATH", strPath);

	auto setup = gcnew System::AppDomainSetup();
	setup->ApplicationBase = System::IO::Path::GetFullPath(curDir + "\\Scripts");
	setup->ShadowCopyFiles = "false"; // !?
	setup->ShadowCopyDirectories = setup->ApplicationBase;

	GTA::WriteLog("Creating AppDomain with base \"{0}\"", setup->ApplicationBase);

	auto appDomainName = "ScriptDomain_" + (curDir->GetHashCode() * System::Environment::TickCount).ToString("X");
	auto appDomainPermissions = gcnew System::Security::PermissionSet(System::Security::Permissions::PermissionState::Unrestricted);

	GTA::ManagedGlobals::g_appDomain = System::AppDomain::CreateDomain(appDomainName, nullptr, setup, appDomainPermissions);
	GTA::ManagedGlobals::g_appDomain->InitializeLifetimeService();

	GTA::WriteLog("Created AppDomain \"{0}\"", GTA::ManagedGlobals::g_appDomain->FriendlyName);

	auto typeScriptDomain = GTA::ScriptDomain::typeid;
	try {
		GTA::ManagedGlobals::g_scriptDomain = static_cast<GTA::ScriptDomain^>(GTA::ManagedGlobals::g_appDomain->CreateInstanceFromAndUnwrap(typeScriptDomain->Assembly->Location, typeScriptDomain->FullName));
	} catch (System::Exception^ ex) {
		GTA::WriteLog("*** Failed to create ScriptDomain: {0}", ex->ToString());
		if (ex->InnerException != nullptr) {
			GTA::WriteLog("*** InnerException: {0}", ex->InnerException->ToString());
		}
		return;
	} catch (...) {
		GTA::WriteLog("*** Failed to create ScriptDomain beacuse of unmanaged exception");
		return;
	}

	GTA::WriteLog("Created ScriptDomain!");

	GTA::ManagedGlobals::g_scriptDomain->FindAllTypes();
}

static bool ManagedScriptInit(int scriptIndex, void* fiberMain, void* fiberScript)
{
	return GTA::ManagedGlobals::g_scriptDomain->ScriptInit(scriptIndex, fiberMain, fiberScript);
}

static bool ManagedScriptExists(int scriptIndex)
{
	if (GTA::ManagedGlobals::g_scriptDomain == nullptr) {
		return false;
	}
	return GTA::ManagedGlobals::g_scriptDomain->ScriptExists(scriptIndex);
}

static int ManagedScriptGetWaitTime(int scriptIndex)
{
	return GTA::ManagedGlobals::g_scriptDomain->ScriptGetWaitTime(scriptIndex);
}

static void ManagedScriptResetWaitTime(int scriptIndex)
{
	GTA::ManagedGlobals::g_scriptDomain->ScriptResetWaitTime(scriptIndex);
}

static void ManagedScriptTick(int scriptIndex)
{
	GTA::ManagedGlobals::g_scriptDomain->ScriptTick(scriptIndex);
}

static void ManagedD3DPresent(void* swapchain)
{
	System::IntPtr ptrSwapchain(swapchain);

	for each (auto script in GTA::ManagedGlobals::g_scriptDomain->m_scripts) {
		try {
			script->OnPresent(ptrSwapchain);
		} catch (System::Exception^ ex) {
			GTA::WriteLog("*** Exception during OnPresent: {0}", ex->ToString());
		}
	}
}

#pragma unmanaged
#include <main.h>
#include <Windows.h>
#include <cstdarg>

struct ScriptFiberInfo
{
	int m_index;
	void* m_fiberMain;
	void* m_fiberScript;

	bool m_initialized;
	bool m_defect;
};

static HMODULE _instance;

static std::vector<ScriptFiberInfo*> _scriptFibers;

static void ScriptMainFiber(LPVOID pv)
{
	ScriptFiberInfo* fi = (ScriptFiberInfo*)pv;

	while (true) {
		if (fi->m_defect) {
			SwitchToFiber(fi->m_fiberMain);
			continue;
		}

		if (!fi->m_initialized) {
			if (ManagedScriptInit(fi->m_index, fi->m_fiberMain, fi->m_fiberScript)) {
				fi->m_initialized = true;
			} else {
				fi->m_defect = true;
			}
			SwitchToFiber(fi->m_fiberMain);
			continue;
		}

		ManagedScriptTick(fi->m_index);
		SwitchToFiber(fi->m_fiberMain);
	}
}

static void ScriptMain(int index)
{
	ScriptFiberInfo fi;
	fi.m_index = index;
	fi.m_fiberMain = GetCurrentFiber();
	fi.m_fiberScript = CreateFiber(0, ScriptMainFiber, (LPVOID)&fi);
	fi.m_initialized = false;
	fi.m_defect = false;
	_scriptFibers.push_back(&fi);

	UnmanagedLogWrite("ScriptMain(%d) -> Initialized: %d, defect: %d (main: %p, script: %p)\n", index, (int)fi.m_initialized, (int)fi.m_defect, fi.m_fiberMain, fi.m_fiberScript);

	while (!ManagedScriptExists(fi.m_index)) {
		scriptWait(0);
	}

	UnmanagedLogWrite("ScriptMain(%d) became active!\n", index);

	while (true) {
		int ms = ManagedScriptGetWaitTime(fi.m_index);
		scriptWait(ms);
		ManagedScriptResetWaitTime(fi.m_index);
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

void RegisterScriptMain(int index)
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
	GTA::WriteLog("SHVDN Pro Initializing");

	auto eventSink = gcnew ManagedEventSink();
	System::AppDomain::CurrentDomain->UnhandledException += gcnew System::UnhandledExceptionEventHandler(eventSink, &ManagedEventSink::OnUnhandledException);
}

static void* _fiberControl;

static void ManagedSHVDNProControl()
{
	void* fiber = CreateFiber(0, [](void*) {
		GTA::WriteLog("Control thread initializing");

		LoadScriptDomain();

		SwitchToFiber(_fiberControl);
	}, nullptr);

	SwitchToFiber(fiber);
}

#pragma unmanaged
static void SHVDNProControl()
{
	_fiberControl = GetCurrentFiber();

	scriptWait(0);
	ManagedSHVDNProControl();
}

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH) {
		if (GetEnvironmentVariableA("SHVDNPro", nullptr, 0) > 0) {
			UnmanagedLogWrite("DllMain attach detected SHVDNPro already running");
			return TRUE;
		}

		SetEnvironmentVariableA("SHVDNPro", "Melissa");

		UnmanagedLogWrite("DllMain DLL_PROCESS_ATTACH\n");

		DisableThreadLibraryCalls(hInstance);
		_instance = hInstance;

		ManagedInitialize();
		scriptRegister(hInstance, SHVDNProControl);
		for (int i = 0; i < 21; i++) {
			RegisterScriptMain(i);
		}
		keyboardHandlerRegister(&ScriptKeyboardMessage);
		presentCallbackRegister(&DXGIPresent);

	} else if (reason == DLL_PROCESS_DETACH) {
		if (GetEnvironmentVariableA("SHVDNPro", nullptr, 0) == 0) {
			UnmanagedLogWrite("DllMain detach detected SHVDNPro not running");
			return TRUE;
		}

		SetEnvironmentVariableA("SHVDNPro", nullptr);

		UnmanagedLogWrite("DllMain DLL_PROCESS_DETACH\n");

		presentCallbackUnregister(&DXGIPresent);
		keyboardHandlerUnregister(&ScriptKeyboardMessage);
		scriptUnregister(hInstance);

		for (auto fi : _scriptFibers) {
			DeleteFiber(fi->m_fiberScript);
		}
	}

	return TRUE;
}
