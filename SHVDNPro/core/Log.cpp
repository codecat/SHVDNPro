#include <Log.h>

void GTA::WriteLog(System::String^ format, ... array<System::Object^>^ args)
{
	// Please note that this is required since we're working in a separate AppDomain 50% of the time
	try {
		auto writerPath = System::IO::Path::ChangeExtension(System::Reflection::Assembly::GetExecutingAssembly()->Location, ".log");
		auto writer = gcnew System::IO::StreamWriter(writerPath, true);
		writer->WriteLine("[{0}] {1}", System::DateTime::Now.ToString("HH:mm:ss.fff"), System::String::Format(format, args));
		delete writer;
	} catch (...) { /* um so shvdn ignores this too.. it's dumb but whatever */ }
}
