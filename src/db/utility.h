/**
 * db/utility.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Utilities for `db` module.
 */

#pragma once

// Core libraries.
#include <xalwart.core/utility.h>

// Module definitions.
#include "./_def_.h"


__ORM_DB_BEGIN__

// TESTME: ce
// Error template for class errors.
template <class T>
inline std::string ce(const std::string& method, const std::string& msg)
{
	return xw::util::demangle(typeid(T).name()) + (method.empty() ? ": " : " > " + method + ": ") + msg;
}

// TESTME: fe
// Error template for function errors.
inline std::string fe(const std::string& function, const std::string& msg)
{
	return (function.empty() ? "" : function + ": ") + msg;
}

__ORM_DB_END__
