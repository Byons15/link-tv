#include "Thread.h"

std::function<void(std::exception_ptr)> bns::Thread::exceptHandler;