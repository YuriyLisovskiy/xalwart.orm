/**
 * _def_.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: main module's definitions.
 */

#pragma once

// Module definitions.
#include <xalwart.core/_def_.h>


// xw::orm
#define __ORM_BEGIN__ __MAIN_NAMESPACE_BEGIN__ namespace orm {
#define __ORM_END__ } __MAIN_NAMESPACE_END__

// xw::orm::abc
#define __ABC_BEGIN__ __ORM_BEGIN__ namespace abc {
#define __ABC_END__ } __ORM_END__

// xw::orm::util
#define __ORM_UTIL_BEGIN__ __ORM_BEGIN__ namespace util {
#define __ORM_UTIL_END__ } __ORM_END__

// xw::orm::meta
#define __ORM_META_BEGIN__ __ORM_BEGIN__ namespace meta {
#define __ORM_META_END__ } __ORM_END__
