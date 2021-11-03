/**
 * db/recorder.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./recorder.h"


__ORM_DB_BEGIN__

void MigrationRecorder::ensure_schema(const IDatabaseConnection* connection) const
{
	auto wrapper = this->backend()->wrap_connection();
	const IDatabaseConnection* db = connection ? connection : wrapper.connection();
	if (this->has_table(db))
	{
		return;
	}

	auto editor = this->backend()->schema_editor();
	ops::CreateTable table(models::Migration::meta_table_name);
	table.column<int>("id", {.primary_key=true, .autoincrement=true});
	table.column<std::string>("name", {.max_len=255, .unique=true});
	table.column<dt::Datetime>("applied");
	ProjectState state;
	table.update_state(state);
	table.forward(editor, state, state, db);
}

__ORM_DB_END__
