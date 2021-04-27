/**
 * db/migration.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./migration.h"


__ORM_DB_BEGIN__

bool Migration::apply(
	const abc::ISQLSchemaEditor* editor,
	const std::function<void()>& success_callback
) const
{
	xw::util::require_non_null(editor, "migration > apply: schema editor is nullptr");
	return xw::util::require_non_null(this->sql_driver)->run_transaction(
		[this, editor, success_callback]() -> bool
		{
			for (const auto& operation : this->operations)
			{
				operation->up(editor);
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
	const abc::ISQLSchemaEditor* editor,
	const std::function<void()>& success_callback
) const
{
	xw::util::require_non_null(editor, "migration > rollback: schema editor is nullptr");
	return xw::util::require_non_null(
		this->sql_driver, "migration > rollback: sql driver is nullptr"
	)->run_transaction(
		[this, editor, success_callback]() -> bool
		{
			for (const auto& operation : this->operations)
			{
				operation->down(editor);
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
