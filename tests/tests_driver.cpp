/**
 * tests_driver.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../src/driver.h"
#include "../src/exceptions.h"

using namespace xw;

struct TestDriver_TestModel : public orm::Model
{
	static constexpr const char* meta_table_name = "test";
};

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
		auto _ = this->driver->make_insert_query(TestDriver_TestModel::meta_table_name, "", {"10,'John'"}),
		orm::QueryError
	);
}

TEST_F(SQLDriverBase_TestCase, make_insert_query_ThrowsEmptyRows)
{
	ASSERT_THROW(
		auto _ = this->driver->make_insert_query(TestDriver_TestModel::meta_table_name, "id,name", {}),
		orm::QueryError
	);
}

TEST_F(SQLDriverBase_TestCase, make_insert_query_InserSingleRow)
{
	std::string expected = "INSERT INTO \"test\" (id, name) VALUES (10, 'John');";
	auto actual = this->driver->make_insert_query(
		TestDriver_TestModel::meta_table_name, "id, name", {"10, 'John'"}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(SQLDriverBase_TestCase, make_insert_query_InserMultipleRows)
{
	std::string expected = "INSERT INTO \"test\" (id, name) VALUES (10, 'John'), (11, 'Steve'), (12, 'Janek');";
	auto actual = this->driver->make_insert_query(
		TestDriver_TestModel::meta_table_name, "id, name", {"10, 'John'", "11, 'Steve'", "12, 'Janek'"}
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
		auto _ = this->driver->make_select_query(
			TestDriver_TestModel::meta_table_name, {}, false, {}, {}, {}, -1, -1, {}, {}
		), orm::QueryError
	);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_DefaultSelect)
{
	std::string expected = R"(SELECT "test"."id" AS "test.id", "test"."name" AS "test.name" FROM "test";)";
	auto actual = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_Distinct)
{
	std::string expected = R"(SELECT DISTINCT "test"."id" AS "test.id", "test"."name" AS "test.name" FROM "test";)";
	auto actual = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, true, {}, {}, {}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

class LeftTestModel : public orm::Model
{
public:
	static constexpr const char* meta_table_name = "left_model";
	static constexpr std::initializer_list<const char*> meta_fields = {
		"id", "name"
	};
};

class RightTestModel : public orm::Model
{
public:
	static constexpr const char* meta_table_name = "right_model";
	static constexpr std::initializer_list<const char*> meta_fields = {
		"id", "name", "left_id"
	};
};

TEST_F(SQLDriverBase_TestCase, make_select_query_Join)
{
	std::string expected = R"(SELECT "test"."id" AS "test.id", "test"."name" AS "test.name" FROM "test" LEFT JOIN "right_model" ON "left_model"."id" = "right_model"."left_id";)";
	auto actual = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, false, {
			orm::q::left<LeftTestModel, RightTestModel>("left_id")
		}, {}, {}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_Where)
{
	std::string expected = R"(SELECT "test"."id" AS "test.id", "test"."name" AS "test.name" FROM "test" WHERE "test"."id" = 1;)";
	auto actual = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, false, {}, {
			orm::q::c<TestDriver_TestModel>("id") == 1
		}, {}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_ComplicatedWhere)
{
	std::string expected = R"(SELECT "test"."id" AS "test.id", "test"."name" AS "test.name" FROM "test" WHERE ("test"."id" = 1 AND "test"."name" < 'John');)";
	auto actual = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, false, {}, {
			orm::q::c<TestDriver_TestModel>("id") == 1 & orm::q::c<TestDriver_TestModel>("name") < "John"
		}, {}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_OrderBy)
{
	std::string expected = R"(SELECT "test"."id" AS "test.id", "test"."name" AS "test.name" FROM "test" ORDER BY "test"."id" ASC, "test"."name" DESC;)";
	auto actual = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {
			orm::q::ascending("id"), orm::q::descending("name")
		}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_Limit)
{
	std::string expected = R"(SELECT "test"."id" AS "test.id", "test"."name" AS "test.name" FROM "test" LIMIT 1;)";
	auto actual = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, 1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_ThrowsOffsetWithoutLimit)
{
	ASSERT_THROW(auto _ = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, -1, 1, {}, {}
	), orm::QueryError);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_Offset)
{
	std::string expected = R"(SELECT "test"."id" AS "test.id", "test"."name" AS "test.name" FROM "test" LIMIT 1 OFFSET 1;)";
	auto actual = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, 1, 1, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_GroupBy)
{
	std::string expected = R"(SELECT "test"."id" AS "test.id", "test"."name" AS "test.name" FROM "test" GROUP BY "test"."id", "test"."name";)";
	auto actual = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, -1, 0, {
			"id", "name"
		}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_ThrowsHavingWithoutGroupBy)
{
	ASSERT_THROW(auto _ = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, -1, 0, {}, {
			orm::q::c<TestDriver_TestModel>("id") == 1
		}
	), orm::QueryError);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_Having)
{
	std::string expected = R"(SELECT "test"."id" AS "test.id", "test"."name" AS "test.name" FROM "test" GROUP BY "test"."id" HAVING "test"."id" = 1;)";
	auto actual = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, -1, 0,
		{"id"},
		orm::q::c<TestDriver_TestModel>("id") == 1
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(SQLDriverBase_TestCase, make_select_query_ComplicatedHaving)
{
	std::string expected = R"(SELECT "test"."id" AS "test.id", "test"."name" AS "test.name" FROM "test" GROUP BY "test"."id" HAVING ("test"."id" = 1 AND "test"."name" < 'John');)";
	auto actual = this->driver->make_select_query(
		TestDriver_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, -1, 0, {
			"id"
		},
		orm::q::c<TestDriver_TestModel>("id") == 1 & orm::q::c<TestDriver_TestModel>("name") < "John"
	);
	ASSERT_EQ(expected, actual);
}
