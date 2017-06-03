#include <cstdio>

#include <Script.h>

#include <ManagedGlobals.h>

#include <Config.h>

#pragma unmanaged
#include <Windows.h>
#undef Yield

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
	m_fiberWait = ms;
	ScriptSwitchToMainFiber(m_fiberMain);
}

void GTA::Script::Yield()
{
	Wait(0);
}

GTA::Script^ GTA::Script::GetExecuting()
{
	void* currentFiber = GetCurrentFiber();

	// I don't know if GetCurrentFiber ever returns null, but whatever
	if (currentFiber == nullptr) {
		return nullptr;
	}

	for each (auto script in GTA::ManagedGlobals::g_scripts) {
		if (script->m_fiberCurrent == currentFiber) {
			return script;
		}
	}

	return nullptr;
}

void GTA::Script::WaitExecuting(int ms)
{
	auto script = GetExecuting();
	if (script == nullptr) {
		throw gcnew System::Exception("Illegal call to WaitExecuting() from a non-script script fiber!");
	}
	script->Wait(ms);
}

void GTA::Script::YieldExecuting()
{
	WaitExecuting(0);
}
