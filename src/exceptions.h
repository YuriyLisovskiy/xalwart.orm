/**
 * exceptions.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 *	- QueryError
 *	- SQLError
 */

#pragma once

// Core libraries.
#include <xalwart.core/exceptions.h>

// Module definitions.
#include "./_def_.h"


__ORM_BEGIN__

// Must be thrown when there are errors during collecting
// data for SQL query generator.
DEF_EXCEPTION_WITH_BASE(QueryError, BaseException, "query error");

// Must be thrown to indicate drivers' errors when running SQL statements.
DEF_EXCEPTION_WITH_BASE(SQLError, BaseException, "sql error");

// Must be thrown if the model does not meet some requirements.
DEF_EXCEPTION_WITH_BASE(ModelError, BaseException, "model error");

DEF_EXCEPTION_WITH_BASE(MigrationsError, BaseException, "migrations error");

__ORM_END__
