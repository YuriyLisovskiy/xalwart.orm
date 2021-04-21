/**
 * db/recorder.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./recorder.h"


__ORM_DB_BEGIN__

void MigrationRecorder::ensure_schema() const
{
	if (this->has_table())
	{
		return;
	}

	auto editor = this->driver()->schema_editor();
	ops::CreateTableOperation table(models::Migration::meta_table_name, editor);
	table.Int("id", {.primary_key=true, .autoincrement=true});
	table.String("name", {.max_len=255, .unique=true});
	table.Datetime("applied");
	table.up(editor);
}

std::map<std::string, models::Migration> MigrationRecorder::applied_migrations() const
{
	std::map<std::string, models::Migration> mapping;
	if (this->has_table())
	{
		auto migrations = this->migrations().all();
		for (auto& migration : migrations)
		{
			mapping[migration.name] = std::move(migration);
		}
	}

	return mapping;
}

__ORM_DB_END__
