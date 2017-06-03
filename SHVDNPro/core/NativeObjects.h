#pragma once

System::UInt64 EncodeObject(System::Object^ obj);
System::Object^ DecodeObject(System::Type^ type, System::UInt64* value);
