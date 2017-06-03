#include <Input.h>

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
		//TODO: Call method for scripts
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
