#pragma once

#include "NativeHashes.h"

namespace GTA
{
	namespace Native
	{
		public ref class Function abstract sealed
		{
		public:
			generic <typename T> static T Call(GTA::Hash hash, ... array<System::Object^>^ arguments);
			static void Call(GTA::Hash hash, ... array<System::Object^>^ arguments);

		internal:
			static System::Collections::Generic::List<System::IntPtr>^ UnmanagedStrings = gcnew System::Collections::Generic::List<System::IntPtr>();

			static System::IntPtr AddStringPool(System::String^ string);
			static void ClearStringPool();
		};
	}
}
