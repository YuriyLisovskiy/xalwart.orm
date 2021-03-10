/**
 * tests_driver.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../src/driver.h"
#include "../src/exceptions.h"

using namespace xw;

class TestDriver : public orm::SQLDriverBase
{
public:
	[[nodiscard]]
	inline std::string run_insert(const std::string& query, bool bulk) const override { return ""; }

	inline void run_select(
		const std::string& query,
		void* container,
		void(*handle_row)(void* container, void* row_map)
	) const override {}

	[[nodiscard]]
	inline std::string name() const override { return "test"; }
};

class SQLDriverBase_TestCase : public ::testing::Test
{
protected:
	orm::SQLDriverBase* driver;

	void SetUp() override
	{
		this->driver = new TestDriver();
	}

	void TearDown() override
	{
		delete this->driver;
	}
};

TEST_F(SQLDriverBase_TestCase, make_insert_query_ThrowsEmptyTableName)
{
	ASSERT_THROW(
		auto _ = this->driver->make_insert_query("", "id,name", {"10,'John'"}),
		orm::QueryError
	);
}

TEST_F(SQLDriverBase_TestCase, make_insert_query_ThrowsEmptyColumns)
{
	ASSERT_THROW(
		auto _ = this->driver->make_insert_query("test", "", {"10,'John'"}),
		orm::QueryError
	);
}

TEST_F(SQLDriverBase_TestCase, make_insert_query_ThrowsEmptyRows)
{
	ASSERT_THROW(
		auto _ = this->driver->make_insert_query("test", "id,name", {}),
		orm::QueryError
	);
}

TEST_F(SQLDriverBase_TestCase, make_insert_query_InserSingleRow)
{
	std::string expected = "INSERT INTO \"test\" (id, name) VALUES (10, 'John');";
	auto actual = this->driver->make_insert_query(
		"test", "id, name", {"10, 'John'"}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(SQLDriverBase_TestCase, make_insert_query_InserMultipleRows)
{
	std::string expected = "INSERT INTO \"test\" (id, name) VALUES (10, 'John'), (11, 'Steve'), (12, 'Janek');";
	auto actual = this->driver->make_insert_query(
		"test", "id, name", {"10, 'John'", "11, 'Steve'", "12, 'Janek'"}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_ThrowsEmptyTable)
{
	ASSERT_THROW(
		auto _ = this->driver->make_select_query("", {}, false, {}, {}, {}, -1, -1, {}, {}),
		orm::QueryError
	);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_ThrowsEmptyColumns)
{
	ASSERT_THROW(
		auto _ = this->driver->make_select_query("test", {}, false, {}, {}, {}, -1, -1, {}, {}),
		orm::QueryError
	);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_DefaultSelect)
{
	std::string expected = R"(SELECT "test"."id" AS "test.id", "test"."name" AS "test.name" FROM "test";)";
	auto actual = this->driver->make_select_query(
		"test", {"id", "name"}, false, {}, {}, {}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

//TEST_F(SQLDriverBase_TestCase, make_select_query_)
//{
//
//}
