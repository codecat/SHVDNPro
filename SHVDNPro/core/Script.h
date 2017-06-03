#pragma once

namespace GTA
{
	public ref class Script abstract
	{
	internal:
		void* m_fiberMain;
		void* m_fiberCurrent;
		int m_fiberWait;

	public:
		Script();

		void Wait(int ms);
		void Yield();

		virtual void OnTick() = 0;

		static Script^ GetExecuting();
		static void WaitExecuting(int ms);
		static void YieldExecuting();
	};
}
