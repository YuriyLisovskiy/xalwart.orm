/**
 * db/executor.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./executor.h"


__ORM_DB_BEGIN__

project_state MigrationExecutor::create_initial_state(bool with_applied_migrations) const
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
				"MigrationExecutor > create_initial_state: detected inconsistency"
				" - you must rollback migrations before deleting them",
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
					"MigrationExecutor > create_initial_state:"
					" detected inconsistency - check if '" + (*m_it)->name() +
					"' migration exists and (or) recorded to the database",
					_ERROR_DETAILS_
				);
			}

			(*m_it)->update_state(state);
		}
	}

	return state;
}

MigrationExecutor::MigrationExecutor(
	orm::abc::ISQLDriver* driver,
	std::list<std::shared_ptr<Migration>> migrations,
	std::function<void(const std::string&, const std::string&)> log_progress
) : recorder(driver), migrations(std::move(migrations)), log_progress(std::move(log_progress))
{
	this->migrations.sort([](const auto& left, const auto& right) -> bool
	{
		if (!left || !right)
		{
			throw NullPointerException(
				"MigrationExecutor: can not compare nullptr migrations",
				_ERROR_DETAILS_
			);
		}

		return left->name() < right->name();
	});
	if (!this->log_progress)
	{
		this->log_progress = [](auto, auto) {};
	}
}

void MigrationExecutor::apply(
	const abc::ISchemaEditor* editor, const std::string& to_migration
) const
{
	this->recorder.ensure_schema();
	auto applied_migrations = this->recorder.applied_migrations();
	auto am_size = applied_migrations.size();
	auto m_size = this->migrations.size();
	if (am_size > m_size)
	{
		throw MigrationsError(
			"MigrationExecutor > apply: detected inconsistency -"
			" you must rollback migrations before deleting them",
			_ERROR_DETAILS_
		);
	}

	this->log_progress("Apply migrations:", "\n");
	if (am_size == m_size)
	{
		this->log_progress(" No migrations to apply.", "\n");
		return;
	}

	// Skip all applied migrations and check for consistency.
	bool all_are_applied = false;
	bool apply_all = to_migration.empty();
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
				"MigrationExecutor > apply: detected inconsistency"
				" - check if '" + (*m_it)->name() +
				"' migration exists and (or) recorded to the database",
				_ERROR_DETAILS_
			);
		}

		if (!apply_all && to_migration == am_it->name)
		{
			all_are_applied = true;
			break;
		}
	}

	if (all_are_applied)
	{
		this->log_progress(" No migrations to apply.", "\n");
		return;
	}

	auto state = this->create_initial_state(true);
	while (m_it != this->migrations.end())
	{
		auto migration = *m_it++;
		auto migration_name = migration->name();
		this->log_progress(" Applying '" + migration_name + "'...", "");
		bool applied = migration->apply(state, editor, [this, migration_name]()
		{
			this->recorder.record_applied(migration_name);
			this->log_progress(" DONE", "\n");
		});
		if (!applied)
		{
			this->log_progress(" FAILED", "\n");
			return;
		}

		if (!apply_all && to_migration == migration_name)
		{
			break;
		}
	}
}

void MigrationExecutor::rollback(
	const abc::ISchemaEditor* editor, const std::string& to_migration
) const
{
	this->recorder.ensure_schema();
	auto applied_migrations = this->recorder.applied_migrations();
	auto am_size = applied_migrations.size();
	auto m_size = this->migrations.size();
	if (am_size > m_size)
	{
		throw MigrationsError(
			"MigrationExecutor > rollback: detected inconsistency -"
			" you must rollback migrations before deleting them from disk",
			_ERROR_DETAILS_
		);
	}

	this->log_progress("Rollback migrations:", "\n");
	bool rollback_all = to_migration.empty();
	bool found = std::find_if(
		applied_migrations.begin(), applied_migrations.end(),
		[to_migration](const auto& m) -> bool { return m.name == to_migration; }
	) != applied_migrations.end();
	if (!rollback_all && !found)
	{
		this->log_progress(" No migrations to rollback.", "\n");
		return;
	}

	std::map<std::string, project_state> states;
	auto state = this->create_initial_state(false);
	auto am_it = applied_migrations.begin();
	for (
		auto m_it = this->migrations.begin();
		m_it != this->migrations.end() && am_it != applied_migrations.end();
		m_it++, am_it++
	)
	{
		auto migration = *m_it;
		if (migration->name() != am_it->name)
		{
			throw MigrationsError(
				"MigrationExecutor > rollback: detected inconsistency"
				" - check if '" + migration->name() +
				"' migration exists and (or) recorded to the database",
				_ERROR_DETAILS_
			);
		}

		states[migration->name()] = state;
		migration->update_state(state);
	}

	auto migrations_to_run = this->migrations;
	auto erase_it = migrations_to_run.begin();
	std::advance(erase_it, am_size);
	if (erase_it != migrations_to_run.end())
	{
		migrations_to_run.erase(erase_it, migrations_to_run.end());
	}

	bool rolled_back_any = false;
	for (auto migration = migrations_to_run.rbegin(); migration != migrations_to_run.rend(); migration++)
	{
		auto migration_name = (*migration)->name();
		if (!rollback_all && migration_name == to_migration)
		{
			break;
		}

		rolled_back_any = true;
		this->log_progress(" Rolling back '" + migration_name + "'...", "");
		bool rolled_back = (*migration)->rollback(
			states[migration_name], editor, [this, migration_name]()
			{
				this->recorder.record_rolled_back(migration_name);
				this->log_progress(" DONE", "\n");
			}
		);
		if (!rolled_back)
		{
			this->log_progress(" FAILED", "\n");
			return;
		}
	}

	if (!rolled_back_any)
	{
		this->log_progress(" No migrations to roll back.", "\n");
	}
}

__ORM_DB_END__
