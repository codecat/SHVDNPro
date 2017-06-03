#include <Tasks/NativeTask.h>

#include <NativeObjects.h>

#pragma unmanaged
#include <main.h>
#pragma managed

GTA::Tasks::NativeTask::NativeTask()
{
	m_hash = 0;
	m_result = nullptr;
}

void GTA::Tasks::NativeTask::Run()
{
	nativeInit(m_hash);
	for each (auto arg in m_arguments) {
		nativePush64(EncodeObject(arg));
	}
	m_result = nativeCall();
}
