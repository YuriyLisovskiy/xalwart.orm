/**
 * queries/mocked_driver.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#pragma once

#include "../../src/backend.h"

using namespace xw;


class MockedConnection : public orm::abc::IDatabaseConnection
{
public:
	[[nodiscard]]
	inline std::string dbms_name() const override
	{
		return "MockedBackend";
	};

	void inline run_query(
		const std::string& sql_query,
		const std::function<void(const std::map<std::string, char*>& /* columns */)>& map_handler,
		const std::function<void(const std::vector<char*>& /* columns */)>& vector_handler
	) const override
	{
	}

	void inline run_query(const std::string& sql_query, std::string& last_row_id) const override
	{
		last_row_id = "1";
	}

	void inline begin_transaction() const override
	{
	}

	void inline end_transaction() const override
	{
	}

	void inline rollback_transaction() const override
	{
	}
};


class MockedBackend : public orm::DefaultSQLBackend
{
public:
	MockedBackend() : orm::DefaultSQLBackend(1, []() -> std::shared_ptr<orm::abc::IDatabaseConnection>
	{
		return std::make_shared<MockedConnection>();
	})
	{
		this->create_pool();
	}

	std::vector<std::string> get_table_names() override
	{
		return {};
	}

	[[nodiscard]]
	inline std::string dbms_name() const override
	{
		return "MockedBackend";
	}
};
