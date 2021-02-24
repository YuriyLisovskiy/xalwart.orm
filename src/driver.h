/**
 * db.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>
#include <map>
#include <functional>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
// TODO

__ORM_BEGIN__

class DbDriver
{
public:
	virtual ~DbDriver() = default;

	virtual void run_select(
		const std::string& query, void* container, void(*handle_row)(void*, void*)
	) = 0;

	// TODO: add insert, update and delete
};

__ORM_END__
