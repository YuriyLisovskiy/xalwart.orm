/**
 * db/migration.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./migration.h"


__ORM_DB_BEGIN__

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
			ce<Migration>("create_table", "columns builder must be initialized"),
			_ERROR_DETAILS_
		);
	}

	build_columns(table_op);
	if (build_constraints)
	{
		build_constraints(table_op);
	}

	this->operations.push_back(std::make_shared<ops::CreateTable>(table_op));
}

bool Migration::apply(
	project_state& state,
	const abc::ISchemaEditor* editor,
	const std::function<void()>& success_callback
) const
{
	xw::util::require_non_null(
		editor, ce<Migration>("apply", "schema editor is nullptr")
	);
	return xw::util::require_non_null(this->sql_driver)->run_transaction(
		[this, editor, success_callback, &state]() -> bool
		{
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

			return true;
		}
	);
}

bool Migration::rollback(
	project_state& state,
	const abc::ISchemaEditor* editor,
	const std::function<void()>& success_callback
) const
{
	xw::util::require_non_null(
		editor,
		ce<Migration>("rollback", "schema editor is nullptr")
	);

	std::list<std::tuple<
		std::shared_ptr<abc::IOperation>, project_state, project_state
	>> ops_to_run;
	auto new_state = state;

	for (const auto& operation : this->operations)
	{
		auto old_state = new_state;
		operation->update_state(new_state);
		ops_to_run.emplace_front(operation, old_state, new_state);
	}

	return xw::util::require_non_null(
		this->sql_driver, ce<Migration>("rollback", "driver is nullptr")
	)->run_transaction(
		[ops_to_run, editor, success_callback]() -> bool
		{
			for (const auto& op : ops_to_run)
			{
				std::get<0>(op)->backward(editor, std::get<2>(op), std::get<1>(op));
			}

			if (success_callback)
			{
				success_callback();
			}

			return true;
		}
	);
}

__ORM_DB_END__
