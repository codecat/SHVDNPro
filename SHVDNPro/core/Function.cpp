#include <Function.h>

#include <NativeObjects.h>
#include <ManagedGlobals.h>

#include <Config.h>

#ifdef THROW_ON_MULTITHREADED_NATIVES
#include <Script.h>
#endif

#pragma unmanaged
#include <main.h>
#pragma managed

generic <typename T> T GTA::Native::Function::Call(GTA::Hash hash, ... array<System::Object^>^ arguments)
{
	//TODO: If calling outside of the executing fiber, queue a NativeTask to an SVHDN control thread

#ifdef THROW_ON_MULTITHREADED_NATIVES
	if (GTA::Script::GetExecuting() == nullptr) {
		throw gcnew System::Exception("Illegal native call outside of main script thread!");
	}
#endif

	nativeInit((UINT64)hash);
	for each (auto arg in arguments) {
		nativePush64(EncodeObject(arg));
	}
	return static_cast<T>(DecodeObject(T::typeid, nativeCall()));
}

void GTA::Native::Function::Call(GTA::Hash hash, ... array<System::Object^>^ arguments)
{
#ifdef THROW_ON_MULTITHREADED_NATIVES
	if (GTA::Script::GetExecuting() == nullptr) {
		throw gcnew System::Exception("Illegal native call outside of main script thread!");
	}
#endif

	nativeInit((UINT64)hash);
	for each (auto arg in arguments) {
		nativePush64(EncodeObject(arg));
	}
	nativeCall();
}

System::IntPtr GTA::Native::Function::AddStringPool(System::String^ string)
{
	auto managedBuffer = System::Text::Encoding::UTF8->GetBytes(string);
	unsigned char* buffer = new unsigned char[managedBuffer->Length + 1];
	buffer[managedBuffer->Length] = '\0';
	System::IntPtr ret(buffer);
	System::Runtime::InteropServices::Marshal::Copy(managedBuffer, 0, ret, managedBuffer->Length);
	GTA::Native::Function::UnmanagedStrings->Add(ret);
	return ret;
}

void GTA::Native::Function::ClearStringPool()
{
	for each (auto ptr in GTA::Native::Function::UnmanagedStrings) {
		delete[] ptr.ToPointer();
	}
	GTA::Native::Function::UnmanagedStrings->Clear();
}
