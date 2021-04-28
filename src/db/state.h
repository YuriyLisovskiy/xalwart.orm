/**
 * db/state.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>
#include <map>

// Core libraries.
#include <xalwart.core/exceptions.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./abc.h"


__ORM_DB_BEGIN__

struct table_state
{
	std::map<std::string, std::tuple<sql_column_type, std::string, constraints_t>> columns{};
	std::map<std::string, foreign_key_constraints_t> foreign_keys{};
};

struct project_state
{
	std::map<std::string, table_state> tables{};
};

__ORM_DB_END__
