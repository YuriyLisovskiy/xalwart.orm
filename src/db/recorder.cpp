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
	ops::CreateTable table(models::Migration::meta_table_name);
	table.column<int>("id", {.primary_key=true, .autoincrement=true});
	table.column<std::string>("name", {.max_len=255, .unique=true});
	table.column<dt::Datetime>("applied");
	project_state state;
	table.update_state(state);
	table.forward(editor, state, state);
}

__ORM_DB_END__
