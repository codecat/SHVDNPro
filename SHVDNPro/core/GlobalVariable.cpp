#include <GlobalVariable.h>

#include <NativeObjects.h>

#include <Math/Vector2.hpp>
#include <Math/Vector3.hpp>

#pragma unmanaged
#include <main.h>
#pragma managed

GTA::Native::GlobalVariable GTA::Native::GlobalVariable::Get(int index)
{
	System::IntPtr address(getGlobalPtr(index));

	if (address == System::IntPtr::Zero)
	{
		throw gcnew System::IndexOutOfRangeException(System::String::Format("The index {0} does not correspond to an existing global variable.", index));
	}

	return GlobalVariable(address);
}

GTA::Native::GlobalVariable::GlobalVariable(System::IntPtr address) : _address(address)
{
}

generic <typename T> T GTA::Native::GlobalVariable::Read()
{
	if (T::typeid == System::String::typeid) {
		const auto size = static_cast<int>(strlen(static_cast<char *>(_address.ToPointer())));
		if (size == 0) {
			return reinterpret_cast<T>(System::String::Empty);
		}

		const auto bytes = gcnew array<System::Byte>(size);
		System::Runtime::InteropServices::Marshal::Copy(_address, bytes, 0, bytes->Length);

		return reinterpret_cast<T>(System::Text::Encoding::UTF8->GetString(bytes));
	}

	return static_cast<T>(DecodeObject(T::typeid, static_cast<System::UInt64*>(_address.ToPointer())));
}

generic <typename T> void GTA::Native::GlobalVariable::Write(T value)
{
	if (T::typeid == System::String::typeid) {
		throw gcnew System::InvalidOperationException("Cannot write string values via 'Write<string>', use 'WriteString' instead.");
	}

	if (T::typeid == Math::Vector2::typeid) {
		const auto val = static_cast<Math::Vector2>(value);
		const auto data = static_cast<float*>(_address.ToPointer());

		data[0] = val.X;
		data[2] = val.Y;
		return;
	}

	if (T::typeid == Math::Vector3::typeid) {
		const auto val = static_cast<Math::Vector3>(value);
		const auto data = static_cast<float*>(_address.ToPointer());

		data[0] = val.X;
		data[2] = val.Y;
		data[4] = val.Z;
		return;
	}

	*static_cast<System::UInt64*>(_address.ToPointer()) = EncodeObject(value);
}

void GTA::Native::GlobalVariable::WriteString(System::String^ value, int maxSize)
{
	if (maxSize % 8 != 0 || maxSize <= 0 || maxSize > 64) {
		throw gcnew System::ArgumentException("The string maximum size should be one of 8, 16, 24, 32 or 64.", "maxSize");
	}

	auto bytes = System::Text::Encoding::UTF8->GetBytes(value);
	auto size = bytes->Length;
	if (size >= maxSize) {
		size = maxSize - 1;
	}

	System::Runtime::InteropServices::Marshal::Copy(bytes, 0, _address, size);
	static_cast<char *>(_address.ToPointer())[size] = '\0';
}

void GTA::Native::GlobalVariable::SetBit(int index)
{
	if (index < 0 || index > 63) {
		throw gcnew System::IndexOutOfRangeException("The bit index has to be between 0 and 63");
	}

	*static_cast<System::UInt64*>(_address.ToPointer()) |= (1ull << index);
}

void GTA::Native::GlobalVariable::ClearBit(int index)
{
	if (index < 0 || index > 63) {
		throw gcnew System::IndexOutOfRangeException("The bit index has to be between 0 and 63");
	}

	*static_cast<System::UInt64*>(_address.ToPointer()) &= ~(1ull << index);
}

bool GTA::Native::GlobalVariable::IsBitSet(int index)
{
	if (index < 0 || index > 63) {
		throw gcnew System::IndexOutOfRangeException("The bit index has to be between 0 and 63");
	}

	return ((*static_cast<System::UInt64*>(_address.ToPointer()) >> index) & 1) != 0;
}

GTA::Native::GlobalVariable GTA::Native::GlobalVariable::GetStructField(int index)
{
	if (index < 0) {
		throw gcnew System::IndexOutOfRangeException("The structure item index cannot be negative.");
	}

	return GlobalVariable(MemoryAddress + (8 * index));
}

array<GTA::Native::GlobalVariable>^ GTA::Native::GlobalVariable::GetArray(int itemSize)
{
	if (itemSize <= 0) {
		throw gcnew System::ArgumentOutOfRangeException("itemSize", "The item size for an array must be positive.");
	}

	int count = Read<int>();

	// Globals are stored in pages that hold a maximum of 65536 items
	if (count < 1 || count >= 65536 / itemSize) {
		throw gcnew System::InvalidOperationException("The variable does not seem to be an array.");
	}

	auto result = gcnew array<GlobalVariable>(count);

	for (int i = 0; i < count; i++) {
		result[i] = GlobalVariable(MemoryAddress + 8 + (8 * itemSize * i));
	}

	return result;
}

GTA::Native::GlobalVariable GTA::Native::GlobalVariable::GetArrayItem(int index, int itemSize)
{
	if (itemSize <= 0) {
		throw gcnew System::ArgumentOutOfRangeException("itemSize", "The item size for an array must be positive.");
	}

	int count = Read<int>();

	// Globals are stored in pages that hold a maximum of 65536 items
	if (count < 1 || count >= 65536 / itemSize) {
		throw gcnew System::InvalidOperationException("The variable does not seem to be an array.");
	}

	if (index < 0 || index >= count) {
		throw gcnew System::IndexOutOfRangeException(System::String::Format("The index {0} was outside the array bounds.", index));
	}

	return GlobalVariable(MemoryAddress + 8 + (8 * itemSize * index));
}
