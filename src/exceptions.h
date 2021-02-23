/**
 * exceptions.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 *	- SQLError
 */

#pragma once

// Core libraries.
#include <xalwart.core/exceptions.h>

// Module definitions.
#include "./_def_.h"


__ORM_BEGIN__

DEF_EXCEPTION_WITH_BASE(QueryError, core::BaseException, "query error");
DEF_EXCEPTION_WITH_BASE(SQLError, core::BaseException, "sql error");

__ORM_END__
