/**
 * db/migration.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./migration.h"


__ORM_DB_BEGIN__

bool Migration::apply(
	ProjectState& state, const abc::ISchemaEditor* editor, const std::function<void()>& success_callback
) const
{
	require_non_null(editor, ce<Migration>("apply", "schema editor is nullptr"));
	auto connection = this->sql_backend->get_connection();
	require_non_null(connection.get(), ce<Migration>("apply", "SQL connection is nullptr"));
	try
	{
		this->apply_unsafe(connection.get(), state, editor, success_callback);
	}
	catch (const BaseException& exc)
	{
		this->rollback_and_release_connection(connection);
		throw exc;
	}
	catch (const std::exception& exc)
	{
		this->rollback_and_release_connection(connection);
		throw exc;
	}

	return true;
}

bool Migration::rollback(
	ProjectState& state, const abc::ISchemaEditor* editor, const std::function<void()>& success_callback
) const
{
	require_non_null(editor, ce<Migration>("rollback", "schema editor is nullptr"));
	auto connection = this->sql_backend->get_connection();
	require_non_null(connection.get(), ce<Migration>("rollback", "connection is nullptr"));
	try
	{
		this->rollback_unsafe(connection.get(), state, editor, success_callback);
	}
	catch (const BaseException& exc)
	{
		this->rollback_and_release_connection(connection);
		throw exc;
	}
	catch (const std::exception& exc)
	{
		this->rollback_and_release_connection(connection);
		throw exc;
	}

	return true;
}

void Migration::create_table(
	const std::string& name,
	const std::function<void(ops::CreateTable&)>& build_columns,
	const std::function<void(ops::CreateTable&)>& build_constraints
)
{
	ops::CreateTable table_op(name);
	if (!build_columns)
	{
		throw NullPointerException(
			ce<Migration>("create_table", "columns builder must be initialized"), _ERROR_DETAILS_
		);
	}

	build_columns(table_op);
	if (build_constraints)
	{
		build_constraints(table_op);
	}

	this->operations.push_back(std::make_shared<ops::CreateTable>(table_op));
}

void Migration::apply_unsafe(
	xw::abc::orm::DatabaseConnection* connection,
	ProjectState& state, const abc::ISchemaEditor* editor, const std::function<void()>& success_callback
) const
{
	connection->begin_transaction();
	for (const auto& operation : this->operations)
	{
		auto old_state = state;
		operation->update_state(state);
		operation->forward(editor, old_state, state);
	}

	if (success_callback)
	{
		success_callback();
	}

	connection->end_transaction();
}

void Migration::rollback_unsafe(
	xw::abc::orm::DatabaseConnection* connection,
	ProjectState& state,
	const abc::ISchemaEditor* editor,
	const std::function<void()>& success_callback
) const
{
	std::list<std::tuple<std::shared_ptr<abc::IOperation>, ProjectState, ProjectState>> ops_to_run;
	auto new_state = state;
	for (const auto& operation : this->operations)
	{
		auto old_state = new_state;
		operation->update_state(new_state);
		ops_to_run.emplace_front(operation, old_state, new_state);
	}

	connection->begin_transaction();
	for (const auto& op : ops_to_run)
	{
		std::get<0>(op)->backward(editor, std::get<2>(op), std::get<1>(op));
	}

	if (success_callback)
	{
		success_callback();
	}

	connection->end_transaction();
}

__ORM_DB_END__
