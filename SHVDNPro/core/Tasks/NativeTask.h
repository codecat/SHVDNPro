#pragma once

#include <IScriptTask.h>
#include <InputArgument.h>

namespace GTA
{
	namespace Tasks
	{
		public ref struct NativeTask : IScriptTask
		{
		public:
			System::UInt64 m_hash;
			System::UInt64* m_result;
			array<System::Object^>^ m_arguments;

		public:
			NativeTask();

			virtual void Run();
		};
	}
}
