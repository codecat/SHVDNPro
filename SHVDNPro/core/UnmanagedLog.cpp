#include <UnmanagedLog.h>

#include <cstdio>
#include <cstdarg>

#pragma unmanaged

void UnmanagedLogWrite(const char* format, ...)
{
	FILE* fh = fopen("SHVDNProUnmanaged.log", "ab");
	if (fh == nullptr) {
		return;
	}

	va_list va;
	va_start(va, format);
	vfprintf(fh, format, va);
	va_end(va);

	fclose(fh);
}
