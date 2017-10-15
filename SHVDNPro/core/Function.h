#pragma once

#include "NativeHashes.h"
#include "InputArgument.h"

namespace GTA
{
	namespace Native
	{
		public ref class Function abstract sealed
		{
		public:
			generic <typename T> static T Call(GTA::Native::Hash hash, ... array<InputArgument^>^ arguments);
			static void Call(GTA::Native::Hash hash, ... array<InputArgument^>^ arguments);

			static System::IntPtr AddStringPool(System::String^ string);

		internal:
			static System::Collections::Generic::List<System::IntPtr>^ UnmanagedStrings = gcnew System::Collections::Generic::List<System::IntPtr>();
			static void ClearStringPool();
		};
	}
}
