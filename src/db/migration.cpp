/**
 * db/migration.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./migration.h"


__ORM_DB_BEGIN__

bool Migration::apply(
	project_state& state,
	const abc::ISQLSchemaEditor* editor,
	const std::function<void()>& success_callback
) const
{
	xw::util::require_non_null(editor, "migration > apply: schema editor is nullptr");
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
	const abc::ISQLSchemaEditor* editor,
	const std::function<void()>& success_callback
) const
{
	xw::util::require_non_null(editor, "migration > rollback: schema editor is nullptr");

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
		this->sql_driver, "migration > rollback: sql driver is nullptr"
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
