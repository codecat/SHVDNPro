#pragma once

namespace GTA
{
	public ref class Script abstract
	{
	internal:
		void* m_fiberMain;
		void* m_fiberCurrent;
		int m_fiberWait;
		System::Collections::Concurrent::ConcurrentQueue<System::Tuple<bool, System::Windows::Forms::Keys>^>^ m_keyboardEvents = gcnew System::Collections::Concurrent::ConcurrentQueue<System::Tuple<bool, System::Windows::Forms::Keys>^>();

	public:
		Script();

		void Wait(int ms);
		void Yield();

		virtual void OnInit();
		virtual void OnTick();
		virtual void OnPresent(System::IntPtr swapchain);
		virtual void OnKeyDown(System::Windows::Forms::KeyEventArgs^ args);
		virtual void OnKeyUp(System::Windows::Forms::KeyEventArgs^ args);

		static Script^ GetExecuting();
		static void WaitExecuting(int ms);
		static void YieldExecuting();

	internal:
		void ProcessOneTick();
	};
}
