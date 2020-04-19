#pragma once

#include <common/config.h>

#include <cassert>
#include <string>

namespace DDS_ROOT_NAMESPACE {

#define DdsVerify assert

using String = std::string;

// untill switch to C++17 or boost::string_view
using StringView = std::string;
} // namespace DDS_ROOT_NAMESPACE
