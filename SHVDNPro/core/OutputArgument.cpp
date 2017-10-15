#include <OutputArgument.h>

#include <NativeObjects.h>

#include <cstring>
#include <cstdlib>

GTA::Native::OutputArgument::OutputArgument()
{
	_storage = malloc(24);
	memset(_storage, 0, 24);
}

GTA::Native::OutputArgument::OutputArgument(Object ^value) : OutputArgument()
{
	*reinterpret_cast<System::UInt64*>(_storage) = EncodeObject(value);
}

GTA::Native::OutputArgument::~OutputArgument()
{
	this->!OutputArgument();
}

generic <typename T> T GTA::Native::OutputArgument::GetResult()
{
	return static_cast<T>(DecodeObject(T::typeid, reinterpret_cast<System::UInt64*>(_storage)));
}

void* GTA::Native::OutputArgument::GetPointer()
{
	return _storage;
}

GTA::Native::OutputArgument::!OutputArgument()
{
	free(_storage);
}
