/**
 * queries/mocked_driver.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#pragma once

#include "../../src/sql_driver.h"

using namespace xw;


class MockedDriver : public orm::DefaultSQLDriver
{
public:
	std::vector<std::string> table_names() const override
	{
		return {};
	}

	inline void run_query(const std::string&) const override
	{
	}

	inline void run_insert(const std::string& query) const override
	{
	}

	inline void run_insert(const std::string& query, std::string& pk) const override
	{
		pk = "1";
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

	inline bool run_transaction(const std::function<bool()>&) const override
	{
		return false;
	}

	[[nodiscard]]
	inline std::string name() const override
	{
		return "MockedDriver";
	}
};
