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
#include "../utility.h"


__ORM_DB_OPERATIONS_BEGIN__

// TESTME: TableOperation
// Base class for operations with table.
class TableOperation : public abc::IOperation
{
protected:
	mutable std::string table_name;

public:
	TableOperation() = default;

	inline explicit TableOperation(std::string name) : table_name(std::move(name))
	{
		if (table_name.empty())
		{
			throw ValueError(
				ce<TableOperation>("", "'name' can not be empty"), _ERROR_DETAILS_
			);
		}
	}

	[[nodiscard]]
	inline const std::string& name() const
	{
		return this->table_name;
	}
};

// TESTME: ColumnOperation
// Base class for operations with columns.
class ColumnOperation : public abc::IOperation
{
protected:
	std::string table_name_;
	std::string column_name;

public:
	ColumnOperation() = default;

	inline explicit ColumnOperation(std::string table_name, std::string column_name) :
		table_name_(std::move(table_name)),
		column_name(std::move(column_name))
	{
		if (this->table_name_.empty())
		{
			throw ValueError(
				ce<ColumnOperation>("", "'table_name' can not be empty"),
				_ERROR_DETAILS_
			);
		}

		if (this->column_name.empty())
		{
			throw ValueError(
				ce<ColumnOperation>("", "'column_name' can not be empty"),
				_ERROR_DETAILS_
			);
		}
	}

	[[nodiscard]]
	inline const std::string& table_name() const
	{
		return this->table_name_;
	}

	[[nodiscard]]
	inline const std::string& name() const
	{
		return this->column_name;
	}
};

__ORM_DB_OPERATIONS_END__
