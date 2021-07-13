/**
 * sql_builder.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./sql_builder.h"


__ORM_BEGIN__

std::string DefaultSQLBuilder::sql_insert(
	const std::string& table_name, const std::string& columns,
	const std::list<std::string>& rows
) const
{
	if (table_name.empty())
	{
		this->_throw_empty_arg("table_name", _ERROR_DETAILS_);
	}

	if (columns.empty())
	{
		this->_throw_empty_arg("columns", _ERROR_DETAILS_);
	}

	auto values = str::join("), (", rows.begin(), rows.end());
	if (values.empty())
	{
		this->_throw_empty_arg("rows", _ERROR_DETAILS_);
	}

	return "INSERT INTO " + util::quote_str(table_name) + " (" + columns + ") VALUES (" + values + ");";
}

std::string DefaultSQLBuilder::sql_select_(
	const std::string& table_name,
	const std::string& columns,
	bool distinct,
	const std::list<q::join_t>& joins,
	const q::condition_t& where_cond,
	const std::list<q::ordering>& order_by_cols,
	long int limit,
	long int offset,
	const std::list<std::string>& group_by_cols,
	const q::condition_t& having_cond
) const
{
	if (table_name.empty())
	{
		this->_throw_empty_arg("table_name", _ERROR_DETAILS_);
	}

	if (columns.empty())
	{
		this->_throw_empty_arg("columns", _ERROR_DETAILS_);
	}

	auto query = std::string("SELECT") + (distinct ? " DISTINCT" : "") + " " + columns +
		" FROM " + util::quote_str(table_name);

	for (const auto& join_row : joins)
	{
		query += " " + (std::string)join_row;
	}

	auto where_str = (std::string)where_cond;
	if (!where_str.empty())
	{
		query += " WHERE " + where_str;
	}

	if (!order_by_cols.empty())
	{
		query += " ORDER BY " + str::join(
			", ", order_by_cols.begin(), order_by_cols.end(),
			[](const auto& ob_column) -> auto
			{
				return (std::string)ob_column;
			}
		);
	}

	if (limit > -1)
	{
		query += " LIMIT " + std::to_string(limit);
	}

	if (offset > 0)
	{
		if (limit < 0)
		{
			throw QueryError(
				"DefaultSQLBuilder: 'offset' is used without 'limit'", _ERROR_DETAILS_
			);
		}

		query += " OFFSET " + std::to_string(offset);
	}

	if (!group_by_cols.empty())
	{
		auto prefix = util::quote_str(table_name) + ".";
		query += " GROUP BY " + str::join(
			", ", group_by_cols.begin(), group_by_cols.end(),
			[prefix](const auto& gb_col) -> auto
			{
				if (gb_col.find('.') == std::string::npos)
				{
					return prefix + util::quote_str(gb_col);
				}

				return gb_col;
			}
		);
	}

	auto having_str = (std::string)having_cond;
	if (!having_str.empty())
	{
		if (group_by_cols.empty())
		{
			throw QueryError(
				"DefaultSQLBuilder: 'having' is used without 'group by'", _ERROR_DETAILS_
			);
		}

		query += " HAVING " + having_str;
	}

	return query + ";";
}

std::string DefaultSQLBuilder::sql_select(
	const std::string& table_name,
	const std::list<std::string>& columns,
	bool distinct,
	const std::list<q::join_t>& joins,
	const q::condition_t& where_cond,
	const std::list<q::ordering>& order_by_cols,
	long int limit,
	long int offset,
	const std::list<std::string>& group_by_cols,
	const q::condition_t& having_cond
) const
{
	auto raw_prefix = table_name + ".";
	auto prefix = util::quote_str(table_name) + ".";
	std::string columns_str;
	for (auto column = columns.begin(); column != columns.end(); column++)
	{
		auto column_str = std::string(*column);
		columns_str += prefix + util::quote_str(column_str);
		columns_str += " AS " + util::quote_str(column_str);
		if (std::next(column) != columns.end())
		{
			columns_str += ", ";
		}
	}

	return this->sql_select_(
		table_name, columns_str, distinct, joins, where_cond,
		order_by_cols, limit, offset, group_by_cols, having_cond
	);
}

std::string DefaultSQLBuilder::sql_update(
	const std::string& table_name,
	const std::string& columns_data,
	const q::condition_t& condition
) const
{
	if (table_name.empty())
	{
		this->_throw_empty_arg("table_name", _ERROR_DETAILS_);
	}

	if (columns_data.empty())
	{
		this->_throw_empty_arg("columns_data", _ERROR_DETAILS_);
	}

	std::string query = "UPDATE " + util::quote_str(table_name) + " SET " + columns_data;
	auto cond_str = (std::string)condition;
	if (!cond_str.empty())
	{
		query += " WHERE " + cond_str;
	}

	return query + ";";
}

std::string DefaultSQLBuilder::sql_delete(
	const std::string& table_name, const q::condition_t& where_cond
) const
{
	if (table_name.empty())
	{
		this->_throw_empty_arg("table_name", _ERROR_DETAILS_);
	}

	std::string query = "DELETE FROM " + util::quote_str(table_name);
	auto where_str = (std::string)where_cond;
	if (!where_str.empty())
	{
		query += " WHERE " + where_str;
	}

	return query + ";";
}

__ORM_END__
