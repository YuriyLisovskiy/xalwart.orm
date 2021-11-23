/**
 * queries/abstract_query.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * 'AbstractQuery' holds pointers to instances of common objects.
 */

#pragma once

// C++ libraries.
#include <string>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../interfaces.h"


__ORM_BEGIN__

// TESTME: AbstractQuery
// TODO: docs for 'AbstractQuery'
template <db::model_based_type ModelType>
class AbstractQuery
{
	static_assert(ModelType::meta_table_name != nullptr, "'meta_table_name' is not initialized");
public:

	// Appends model's pk to deletion list.
	explicit AbstractQuery(
		const IDatabaseConnection* connection, ISQLQueryBuilder* builder
	) : db_connection(connection), query_builder(builder)
	{
		require_non_null(this->db_connection, "Database connection is nullptr", _ERROR_DETAILS_);
		require_non_null(this->query_builder, "SQL query builder is nullptr", _ERROR_DETAILS_);
	};

	[[nodiscard]]
	virtual std::string to_sql() const = 0;

protected:
	const IDatabaseConnection* db_connection = nullptr;
	ISQLQueryBuilder* query_builder = nullptr;
};

__ORM_END__
