#include <Input.h>

#include <ManagedGlobals.h>

#include <Windows.h>

void ManagedScriptKeyboardMessage(unsigned long key, unsigned short repeats, unsigned char scanCode, bool isExtended, bool isWithAlt, bool wasDownBefore, bool isUpNow)
{
	if (key >= (unsigned long)GTA::Input::_keyboardState->Length) {
		return;
	}

	bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
	bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
	bool status = !isUpNow;

	GTA::Input::_keyboardState[key] = status;

	if (GTA::Input::_captureKeyboardEvents) {
		auto wfkey = (System::Windows::Forms::Keys)key;
		if (ctrl) {
			wfkey = wfkey | System::Windows::Forms::Keys::Control;
		}
		if (shift) {
			wfkey = wfkey | System::Windows::Forms::Keys::Shift;
		}
		if (isWithAlt) {
			wfkey = wfkey | System::Windows::Forms::Keys::Alt;
		}

		auto args = gcnew System::Windows::Forms::KeyEventArgs(wfkey);
		auto eventinfo = gcnew System::Tuple<bool, System::Windows::Forms::KeyEventArgs^>(status, args);

		for each (auto script in GTA::ManagedGlobals::g_scripts) {
			script->m_keyboardEvents->Enqueue(eventinfo);
		}
	}

	//TODO: API for scancodes or WM_CHAR text input?
}

bool GTA::Input::IsKeyPressed(System::Windows::Forms::Keys key)
{
	return _keyboardState[(int)key];
}

void GTA::Input::PauseKeyboardEvents(bool paused)
{
	GTA::Input::_captureKeyboardEvents = !paused;
}
