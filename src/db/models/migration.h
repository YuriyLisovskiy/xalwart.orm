/**
 * db/models/migration.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Migration model for recording applied migrations to the database.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../model.h"


__ORM_DB_MODELS_BEGIN__

// TESTME: Migration
class Migration final : public Model
{
public:
	size_t id{};
	std::string name;
	dt::Datetime applied;

	static constexpr const char* meta_table_name = "xalwart_migrations";

	inline static const std::tuple meta_columns = {
		make_pk_column_meta("id", &Migration::id),
		make_column_meta("name", &Migration::name),
		make_column_meta("applied", &Migration::applied)
	};

	Migration() = default;

	inline explicit Migration(std::string name) :
		name(std::move(name)), applied(dt::Datetime::utc_now())
	{
	}

	[[nodiscard]]
	inline std::string to_string() const override
	{
		return this->is_null() ? "null" : (
			"Migration " + this->name + " at " + this->applied.strftime(db::DEFAULT_DATETIME_FORMAT)
		);
	}

	inline void __orm_set_column__(const std::string& column_name, const char* data) override
	{
		this->__orm_set_column_data__(Migration::meta_columns, column_name, data);
	}
};

__ORM_DB_MODELS_END__
