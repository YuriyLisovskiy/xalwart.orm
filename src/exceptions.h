/**
 * exceptions.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 *	- MigrationsError
 *	- ModelError
 *	- QueryError
 *	- SQLError
 */

#pragma once

// Base libraries.
#include <xalwart.base/exceptions.h>

// Module definitions.
#include "./_def_.h"


__ORM_BEGIN__

DEF_EXCEPTION_WITH_BASE(MigrationsError, BaseException, "migrations error", "orm::");

// Must be thrown if the table does not meet some requirements.
DEF_EXCEPTION_WITH_BASE(ModelError, BaseException, "model error", "orm::");

// Must be thrown when there are errors during driver's implementations.
DEF_EXCEPTION_WITH_BASE(DatabaseError, BaseException, "database error", "orm::");

// Must be thrown when there are errors during collecting
// data for SQL query generator.
DEF_EXCEPTION_WITH_BASE(QueryError, DatabaseError, "query error", "orm::");

// Must be thrown to indicate drivers' errors when running SQL statements.
DEF_EXCEPTION_WITH_BASE(SQLError, DatabaseError, "sql error", "orm::");

__ORM_END__
