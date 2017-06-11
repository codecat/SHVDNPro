#pragma once

// Checking for this gives a little bit of overhead when calling natives that we'd preferably avoid
#define THROW_ON_MULTITHREADED_NATIVES

// Logs all native calls to the logfile
//#define LOG_ALL_NATIVES

// Throw an exception if we're trying to yield from the wrong fiber
#define THROW_ON_WRONG_FIBER_YIELD
