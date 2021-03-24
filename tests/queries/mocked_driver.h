/**
 * queries/mocked_driver.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#pragma once

#include "../../src/driver.h"

using namespace xw;


class MockedDriver : public orm::SQLDriverBase
{
public:
	[[nodiscard]]
	inline std::string run_insert(const std::string& query) const override
	{
		return "1";
	}

	inline void run_select(
		const std::string& query,
		void* container,
		void(*handle_row)(void* container, void* row_map)
	) const override
	{
	}

	inline void run_update(const std::string& query, bool batch) const override
	{
	}

	inline void run_delete(const std::string& query) const override
	{
	}

	[[nodiscard]]
	inline std::string name() const override
	{
		return "MockedDriver";
	}
};
