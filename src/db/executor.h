/**
 * db/executor.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./recorder.h"
#include "./migration.h"


__ORM_DB_BEGIN__

// TESTME: MigrationExecutor
// TODO: docs for 'MigrationExecutor'
class MigrationExecutor
{
public:
	explicit MigrationExecutor(
		orm::abc::ISQLBackend* backend,
		std::list<std::shared_ptr<Migration>> migrations,
		std::function<void(const std::string&, const std::string&)> log_progress=nullptr
	);

	void apply(const abc::ISchemaEditor* editor, const std::string& to_migration="") const;

	void rollback(const abc::ISchemaEditor* editor, const std::string& to_migration="") const;

protected:
	MigrationRecorder recorder;

	std::list<std::shared_ptr<Migration>> migrations;

	std::function<void(const std::string&, const std::string&)> log_progress;

	[[nodiscard]]
	inline ProjectState create_initial_state(bool with_applied_migrations=false) const;
};

__ORM_DB_END__
