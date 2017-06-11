#include <cstdio>

#include <Script.h>

#include <ManagedGlobals.h>
#include <Log.h>

#include <Config.h>

#pragma unmanaged
#include <Windows.h>
#undef Yield

#include <UnmanagedLog.h>

static void ScriptSwitchToMainFiber(void* fiber)
{
	SwitchToFiber(fiber);
}
#pragma managed

GTA::Script::Script()
{
	m_fiberMain = nullptr;
	m_fiberWait = 0;
}

void GTA::Script::Wait(int ms)
{
#ifdef THROW_ON_WRONG_FIBER_YIELD
	if (GetCurrentFiber() != m_fiberCurrent) {
		throw gcnew System::Exception(System::String::Format("Illegal fiber wait {0} from invalid thread:\n{1}", ms, System::Environment::StackTrace));
	}
#endif

	m_fiberWait = ms;
	ScriptSwitchToMainFiber(m_fiberMain);
}

void GTA::Script::Yield()
{
	Wait(0);
}

void GTA::Script::OnInit()
{
}

void GTA::Script::OnTick()
{
}

void GTA::Script::OnPresent(System::IntPtr swapchain)
{
}

void GTA::Script::OnKeyDown(System::Windows::Forms::KeyEventArgs^ args)
{
}

void GTA::Script::OnKeyUp(System::Windows::Forms::KeyEventArgs^ args)
{
}

GTA::Script^ GTA::Script::GetExecuting()
{
	void* currentFiber = GetCurrentFiber();

	// I don't know if GetCurrentFiber ever returns null, but whatever
	if (currentFiber == nullptr) {
		return nullptr;
	}

	for each (auto script in GTA::ManagedGlobals::g_scriptDomain->m_scripts) {
		if (script != nullptr && script->m_fiberCurrent == currentFiber) {
			return script;
		}
	}

	return nullptr;
}

void GTA::Script::WaitExecuting(int ms)
{
	auto script = GetExecuting();
	if (script == nullptr) {
		throw gcnew System::Exception("Illegal call to WaitExecuting() from a non-script fiber!");
	}
	script->Wait(ms);
}

void GTA::Script::YieldExecuting()
{
	WaitExecuting(0);
}

void GTA::Script::ProcessOneTick()
{
	System::Tuple<bool, System::Windows::Forms::KeyEventArgs^>^ ev = nullptr;

	while (m_keyboardEvents->TryDequeue(ev)) {
		try {
			if (ev->Item1) {
				OnKeyDown(ev->Item2);
			} else {
				OnKeyUp(ev->Item2);
			}
		} catch (System::Exception^ ex) {
			if (ev->Item1) {
				GTA::WriteLog("*** Exception during OnKeyDown: {0}", ex->ToString());
			} else {
				GTA::WriteLog("*** Exception during OnKeyUp: {0}", ex->ToString());
			}
		}
	}

	try {
		OnTick();
	} catch (System::Exception^ ex) {
		GTA::WriteLog("*** Exception during OnTick: {0}", ex->ToString());
	} catch (...) {
		GTA::WriteLog("*** Unmanaged exception during OnTick in {0}", GetType()->FullName);
	}
}
