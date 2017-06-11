#include <Function.h>

#include <NativeObjects.h>
#include <ManagedGlobals.h>
#include <Log.h>

#include <Config.h>

#ifdef THROW_ON_MULTITHREADED_NATIVES
#include <Script.h>
#endif

#pragma unmanaged
#include <main.h>
#pragma managed

static void LogNative(System::String^ type, GTA::Hash hash, array<System::Object^>^ args)
{
	/*
	GTA::ManagedGlobals::g_logWriter->Write("{0:HH\\:mm\\:ss\\.fff} Native {1} {2:X16} {3}", System::DateTime::Now, type, (System::UInt64)hash, hash);
	for each (auto arg in args) {
		GTA::ManagedGlobals::g_logWriter->Write(", \"{0}\" ({1})", arg, arg->GetType()->FullName);
	}
	*/
	//TODO
	GTA::WriteLog("LogNative not implemented atm.. pls fix");
}

static void LogNativeOK()
{
	GTA::WriteLog(" OK!");
}

generic <typename T> T GTA::Native::Function::Call(GTA::Hash hash, ... array<System::Object^>^ arguments)
{
	//TODO: If calling outside of the executing fiber, queue a NativeTask to an SVHDN control thread

#ifdef THROW_ON_MULTITHREADED_NATIVES
	if (GTA::Script::GetExecuting() == nullptr) {
		throw gcnew System::Exception("Illegal native call outside of main script thread!");
	}
#endif

#ifdef LOG_ALL_NATIVES
	LogNative("return invoke", hash, arguments);
#endif

	nativeInit((UINT64)hash);
	for each (auto arg in arguments) {
		nativePush64(EncodeObject(arg));
	}
	auto ret = static_cast<T>(DecodeObject(T::typeid, nativeCall()));

#ifdef LOG_ALL_NATIVES
	LogNativeOK();
#endif

	return ret;
}

void GTA::Native::Function::Call(GTA::Hash hash, ... array<System::Object^>^ arguments)
{
#ifdef THROW_ON_MULTITHREADED_NATIVES
	if (GTA::Script::GetExecuting() == nullptr) {
		throw gcnew System::Exception("Illegal native call outside of main script thread!");
	}
#endif

#ifdef LOG_ALL_NATIVES
	LogNative("invoke", hash, arguments);
#endif

	nativeInit((UINT64)hash);
	for each (auto arg in arguments) {
		nativePush64(EncodeObject(arg));
	}
	nativeCall();

#ifdef LOG_ALL_NATIVES
	LogNativeOK();
#endif
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
