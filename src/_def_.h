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

// xw::orm::q
#define __Q_BEGIN__ __ORM_BEGIN__ namespace q {
#define __Q_END__ } __ORM_END__

__ORM_BEGIN__

template <typename T>
concept OperatorValueType = std::is_fundamental_v<T> ||
                            std::is_same_v<T, std::string> ||
							std::is_same_v<T, const char*>;

__ORM_END__
