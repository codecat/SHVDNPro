#include <NativeObjects.h>

#include <Function.h>

#include <INativeValue.h>
#include <OutputArgument.h>

#include <Vector2.hpp>
#include <Vector3.hpp>

#include <ManagedGlobals.h>
#include <Log.h>

#include <cstring>

System::UInt64 EncodeObject(System::Object^ obj)
{
	if (System::Object::ReferenceEquals(obj, nullptr)) {
		return 0;
	}

	auto type = obj->GetType();

	if (type->IsEnum) {
		obj = System::Convert::ChangeType(obj, type = System::Enum::GetUnderlyingType(type));
	}

	if (type == System::Boolean::typeid) {
		return static_cast<bool>(obj) ? 1 : 0;

	} else if (type == System::Byte::typeid) {
		return static_cast<System::Byte>(obj);

	} else if (type == System::Int16::typeid) {
		return static_cast<System::Int16>(obj);

	} else if (type == System::UInt16::typeid) {
		return static_cast<System::UInt16>(obj);

	} else if (type == System::Int32::typeid) {
		return static_cast<System::Int32>(obj);

	} else if (type == System::UInt32::typeid) {
		return static_cast<System::UInt32>(obj);

	} else if (type == System::Int64::typeid) {
		return static_cast<System::Int64>(obj);

	} else if (type == System::UInt64::typeid) {
		return static_cast<System::UInt64>(obj);

	} else if (type == System::Single::typeid) {
		return System::BitConverter::ToUInt32(System::BitConverter::GetBytes(static_cast<float>(obj)), 0);

	} else if (type == System::Double::typeid) {
		return System::BitConverter::ToUInt32(System::BitConverter::GetBytes(static_cast<float>(static_cast<double>(obj))), 0);

	} else if (type == System::String::typeid) {
		return GTA::Native::Function::AddStringPool(static_cast<System::String^>(obj)).ToInt64();

	} else if (type == System::IntPtr::typeid) {
		return static_cast<System::IntPtr>(obj).ToInt64();

	} else if (type == GTA::Native::OutputArgument::typeid) {
		return System::IntPtr(static_cast<GTA::Native::OutputArgument^>(obj)->GetPointer()).ToInt64();

	} else if (GTA::Native::INativeValue::typeid->IsAssignableFrom(type)) {
		return static_cast<GTA::Native::INativeValue^>(obj)->NativeValue;
	}

	GTA::WriteLog("*** Tried encoding unhandled type {0}:\n{1}", type->FullName, System::Environment::StackTrace);
	return 0;
}

System::Object^ DecodeObject(System::Type^ type, System::UInt64* value)
{
	if (type->IsEnum) {
		type = System::Enum::GetUnderlyingType(type);
	}

	if (type == System::Boolean::typeid) {
		return *reinterpret_cast<const int*>(value) != 0;

	} else  if (type == System::Int32::typeid) {
		return *reinterpret_cast<const System::Int32*>(value);

	} else if (type == System::UInt32::typeid) {
		return *reinterpret_cast<const System::UInt32*>(value);

	} else if (type == System::Int64::typeid) {
		return *reinterpret_cast<const System::Int64*>(value);

	} else if (type == System::UInt64::typeid) {
		return *reinterpret_cast<const System::UInt64*>(value);

	} else if (type == System::Single::typeid) {
		return *reinterpret_cast<const float*>(value);

	} else if (type == System::Double::typeid) {
		return static_cast<double>(*reinterpret_cast<const float*>(value));

	} else if (type == System::String::typeid) {
		const auto address = reinterpret_cast<char*>(*value);

		if (address == nullptr) {
			return System::String::Empty;
		}

		const auto size = static_cast<int>(strlen(address));

		if (size == 0) {
			return System::String::Empty;
		}

		const auto bytes = gcnew array<System::Byte>(size);
		System::Runtime::InteropServices::Marshal::Copy(System::IntPtr(address), bytes, 0, bytes->Length);
		return System::Text::Encoding::UTF8->GetString(bytes);

	} if (type == System::IntPtr::typeid) {
		return System::IntPtr(*reinterpret_cast<const System::Int64*>(value));

	} else if (type == GTA::Math::Vector2::typeid) {
		const auto data = reinterpret_cast<const float*>(value);
		return gcnew GTA::Math::Vector2(data[0], data[2]);

	} else if (type == GTA::Math::Vector3::typeid) {
		const auto data = reinterpret_cast<const float*>(value);
		return gcnew GTA::Math::Vector3(data[0], data[2], data[4]);

	} else if (GTA::Native::INativeValue::typeid->IsAssignableFrom(type)) {
		// Warning: Requires classes implementing 'INativeValue' to repeat all constructor work in the setter of 'NativeValue'
		auto ret = static_cast<GTA::Native::INativeValue^>(System::Runtime::Serialization::FormatterServices::GetUninitializedObject(type));
		ret->NativeValue = *value;
		return ret;
	}

	throw gcnew System::InvalidCastException(System::String::Format("Unable to cast native value to object of type '{0}'", type->FullName));
}
