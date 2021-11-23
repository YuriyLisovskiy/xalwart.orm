/**
 * _def_.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Definitions of main module.
 */

#pragma once

// Module definitions.
#include <xalwart.base/_def_.h>

__ORM_BEGIN__

namespace v
{
inline const auto version = Version("0.0.0");
};

__ORM_END__

// xw::orm::util
#define __ORM_UTILITY_BEGIN__ __ORM_BEGIN__ namespace util {
#define __ORM_UTILITY_END__ } __ORM_END__
