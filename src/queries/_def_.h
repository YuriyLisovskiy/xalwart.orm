/**
 * query/_def_.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: query module's definitions.
 */

#pragma once

// Module definitions.
#include "../_def_.h"

// xw::orm::q
#define __Q_BEGIN__ __ORM_BEGIN__ namespace q {
#define __Q_END__ } __ORM_END__

__ORM_BEGIN__

template <typename T>
concept OperatorValueType = std::is_fundamental_v<T> ||
	std::is_same_v<T, std::string> || std::is_same_v<T, const char*>;

__ORM_END__
