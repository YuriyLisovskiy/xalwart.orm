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
class MigrationExecutor
{
protected:
	MigrationRecorder recorder;

	std::list<std::shared_ptr<Migration>> migrations;

	std::function<void(const std::string&, const std::string&)> log_progress;

	[[nodiscard]]
	inline project_state create_initial_state(bool with_applied_migrations=false) const
	{
		project_state state;
		if (with_applied_migrations)
		{
			this->recorder.ensure_schema();
			auto applied_migrations = this->recorder.applied_migrations();
			auto am_size = applied_migrations.size();
			auto m_size = this->migrations.size();
			if (am_size > m_size)
			{
				throw MigrationsError(
					"detected inconsistency: you must rollback migrations before deleting them",
					_ERROR_DETAILS_
				);
			}

			decltype(migrations)::const_iterator m_it;
			auto am_it = applied_migrations.begin();
			for (
				m_it = this->migrations.begin();
				m_it != this->migrations.end() && am_it != applied_migrations.end();
				m_it++, am_it++
			)
			{
				if ((*m_it)->name() != am_it->name)
				{
					throw MigrationsError(
						"detected inconsistency: check if '" + (*m_it)->name() +
						"' migration exists and (or) recorded to the database",
						_ERROR_DETAILS_
					);
				}

				(*m_it)->update_state(state);
			}
		}

		return state;
	}

public:
	explicit MigrationExecutor(
		orm::abc::ISQLDriver* driver,
		std::list<std::shared_ptr<Migration>> migrations,
		std::function<void(const std::string&, const std::string&)> log_progress=nullptr
	);

	void apply(
		const abc::ISQLSchemaEditor* editor, const std::string& to_migration=""
	) const;

	void rollback(
		const abc::ISQLSchemaEditor* editor, const std::string& to_migration=""
	) const;
};

__ORM_DB_END__
