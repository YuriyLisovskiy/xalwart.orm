/**
 * db/operations/base.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// Core libraries.
#include <xalwart.core/string_utils.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../abc.h"


__ORM_DB_OPERATIONS_BEGIN__

// TESTME: TableOperation
// Base class for operations with table.
class TableOperation : public abc::IOperation
{
protected:
	std::string table_name;

public:
	TableOperation() = default;

	inline explicit TableOperation(std::string name) : table_name(std::move(name))
	{
	}

	[[nodiscard]]
	inline const std::string& name() const
	{
		return this->table_name;
	}

	[[nodiscard]]
	inline std::string name_lower() const
	{
		return str::lower(this->table_name);
	}
};

__ORM_DB_OPERATIONS_END__
