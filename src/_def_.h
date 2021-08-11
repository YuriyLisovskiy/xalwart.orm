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

// xw::orm
#define __ORM_BEGIN__ __MAIN_NAMESPACE_BEGIN__ namespace orm {
#define __ORM_END__ } __MAIN_NAMESPACE_END__

__ORM_BEGIN__

namespace v
{
const inline auto version = Version("0.0.0");
};

__ORM_END__

// xw::orm::abc
#define __ORM_ABC_BEGIN__ __ORM_BEGIN__ namespace abc {
#define __ORM_ABC_END__ } __ORM_END__

// xw::orm::util
#define __ORM_UTILITY_BEGIN__ __ORM_BEGIN__ namespace util {
#define __ORM_UTILITY_END__ } __ORM_END__
