#pragma once
#include "config.h"
#include <stdexcept>

#define FLUENTX_THROW_ERROR(x) if (FLUENTX_ENABLE_RUNTIME_ERRORS == 1) throw std::runtime_error(x);

