#include "common.h"
#include "soldier_pool.h"

Soldiers& Singleton()
{
	static Soldiers singleton;
	return singleton;
}