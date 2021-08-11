/**
 * tests_driver.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../src/sql_driver.h"

using namespace xw;

struct TestBuilder_TestModel : public orm::db::Model
{
	int id{};
	std::string name;

	static constexpr const char* meta_table_name = "test";

	inline static const std::tuple meta_columns = {
		orm::db::make_column_meta("id", &TestBuilder_TestModel::id),
		orm::db::make_column_meta("name", &TestBuilder_TestModel::name)
	};

	inline void __set_attr__(const char* attr_name, const void* data) override
	{
		this->set_attribute_to(TestBuilder_TestModel::meta_columns, attr_name, data);
	}

	[[nodiscard]]
	inline std::shared_ptr<const Object> __get_attr__(const char* attr_name) const override
	{
		return this->get_attribute_from(TestBuilder_TestModel::meta_columns, attr_name);
	}
};

class DefaultSQLBuilder_TestCase : public ::testing::Test
{
protected:
	orm::DefaultSQLBuilder sql_builder;
};

TEST_F(DefaultSQLBuilder_TestCase, make_insert_query_ThrowsEmptyTableName)
{
	ASSERT_THROW(
		auto _ = this->sql_builder.sql_insert("", "id,name", {"10,'John'"}),
		orm::QueryError
	);
}

TEST_F(DefaultSQLBuilder_TestCase, make_insert_query_ThrowsEmptyColumns)
{
	ASSERT_THROW(
		auto _ = this->sql_builder.sql_insert(TestBuilder_TestModel::meta_table_name, "", {"10,'John'"}),
		orm::QueryError
	);
}

TEST_F(DefaultSQLBuilder_TestCase, make_insert_query_ThrowsEmptyRows)
{
	ASSERT_THROW(
		auto _ = this->sql_builder.sql_insert(TestBuilder_TestModel::meta_table_name, "id,name", {}),
		orm::QueryError
	);
}

TEST_F(DefaultSQLBuilder_TestCase, make_insert_query_InserSingleRow)
{
	std::string expected = "INSERT INTO \"test\" (id, name) VALUES (10, 'John');";
	auto actual = this->sql_builder.sql_insert(
		TestBuilder_TestModel::meta_table_name, "id, name", {"10, 'John'"}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_insert_query_InserMultipleRows)
{
	std::string expected = "INSERT INTO \"test\" (id, name) VALUES (10, 'John'), (11, 'Steve'), (12, 'Janek');";
	auto actual = this->sql_builder.sql_insert(
		TestBuilder_TestModel::meta_table_name, "id, name", {"10, 'John'", "11, 'Steve'", "12, 'Janek'"}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_ThrowsEmptyTable)
{
	ASSERT_THROW(
		auto _ = this->sql_builder.sql_select("", {}, false, {}, {}, {}, -1, -1, {}, {}),
		orm::QueryError
	);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_ThrowsEmptyColumns)
{
	ASSERT_THROW(
		auto _ = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {}, false, {}, {}, {}, -1, -1, {}, {}
	), orm::QueryError
	);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_DefaultSelect)
{
	std::string expected = R"(SELECT "test"."id" AS "id", "test"."name" AS "name" FROM "test";)";
	auto actual = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_Distinct)
{
	std::string expected = R"(SELECT DISTINCT "test"."id" AS "id", "test"."name" AS "name" FROM "test";)";
	auto actual = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, true, {}, {}, {}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

class LeftTestModel : public orm::db::Model
{
public:
	int id{};
	std::string name;

	static constexpr const char* meta_table_name = "left_model";

	inline static const std::tuple meta_columns = {
		orm::db::make_pk_column_meta("id", &LeftTestModel::id),
		orm::db::make_column_meta("name", &LeftTestModel::name)
	};

	inline void __set_attr__(const char* attr_name, const void* data) override
	{
		this->set_attribute_to(LeftTestModel::meta_columns, attr_name, data);
	}

	[[nodiscard]]
	inline std::shared_ptr<const Object> __get_attr__(const char* attr_name) const override
	{
		return this->get_attribute_from(LeftTestModel::meta_columns, attr_name);
	}
};

class RightTestModel : public orm::db::Model
{
public:
	int id{};
	std::string name;
	int left_id{};

	static constexpr const char* meta_table_name = "right_model";

	inline static const std::tuple meta_columns = {
		orm::db::make_pk_column_meta("id", &RightTestModel::id),
		orm::db::make_column_meta("name", &RightTestModel::name),
		orm::db::make_column_meta("left_id", &RightTestModel::left_id)
	};

	inline void __set_attr__(const char* attr_name, const void* data) override
	{
		this->set_attribute_to(RightTestModel::meta_columns, attr_name, data);
	}

	[[nodiscard]]
	inline std::shared_ptr<const Object> __get_attr__(const char* attr_name) const override
	{
		return this->get_attribute_from(RightTestModel::meta_columns, attr_name);
	}
};

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_Join)
{
	std::string expected = R"(SELECT "test"."id" AS "id", "test"."name" AS "name" FROM "test" LEFT JOIN "right_model" ON "left_model"."id" = "right_model"."left_id";)";
	auto actual = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, false, {
			orm::q::left_on<LeftTestModel, RightTestModel>("left_id")
		}, {}, {}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_Where)
{
	std::string expected = R"(SELECT "test"."id" AS "id", "test"."name" AS "name" FROM "test" WHERE "test"."id" = 1;)";
	auto actual = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, false, {}, {
			orm::q::c(&TestBuilder_TestModel::id) == 1
		}, {}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_ComplicatedWhere)
{
	std::string expected = R"(SELECT "test"."id" AS "id", "test"."name" AS "name" FROM "test" WHERE ("test"."id" = 1 AND "test"."name" < 'John');)";
	auto actual = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, false, {}, {
			orm::q::c(&TestBuilder_TestModel::id) == 1 & orm::q::c(&TestBuilder_TestModel::name) < "John"
		}, {}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_OrderBy)
{
	std::string expected = R"(SELECT "test"."id" AS "id", "test"."name" AS "name" FROM "test" ORDER BY "test"."id" ASC, "test"."name" DESC;)";
	auto actual = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {
			orm::q::asc(&TestBuilder_TestModel::id), orm::q::desc(&TestBuilder_TestModel::name)
		}, -1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_Limit)
{
	std::string expected = R"(SELECT "test"."id" AS "id", "test"."name" AS "name" FROM "test" LIMIT 1;)";
	auto actual = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, 1, 0, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_ThrowsOffsetWithoutLimit)
{
	ASSERT_THROW(auto _ = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, -1, 1, {}, {}
	), orm::QueryError);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_Offset)
{
	std::string expected = R"(SELECT "test"."id" AS "id", "test"."name" AS "name" FROM "test" LIMIT 1 OFFSET 1;)";
	auto actual = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, 1, 1, {}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_GroupBy)
{
	std::string expected = R"(SELECT "test"."id" AS "id", "test"."name" AS "name" FROM "test" GROUP BY "test"."id", "test"."name";)";
	auto actual = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, -1, 0, {
			"id", "name"
		}, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_ThrowsHavingWithoutGroupBy)
{
	ASSERT_THROW(auto _ = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, -1, 0, {}, {
			orm::q::c(&TestBuilder_TestModel::id) == 1
		}
	), orm::QueryError);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_Having)
{
	std::string expected = R"(SELECT "test"."id" AS "id", "test"."name" AS "name" FROM "test" GROUP BY "test"."id" HAVING "test"."id" = 1;)";
	auto actual = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, -1, 0,
		{"id"},
		orm::q::c(&TestBuilder_TestModel::id) == 1
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_select_query_ComplicatedHaving)
{
	std::string expected = R"(SELECT "test"."id" AS "id", "test"."name" AS "name" FROM "test" GROUP BY "test"."id" HAVING ("test"."id" = 1 AND "test"."name" < 'John');)";
	auto actual = this->sql_builder.sql_select(
		TestBuilder_TestModel::meta_table_name, {"id", "name"}, false, {}, {}, {}, -1, 0, {
			"id"
		},
		orm::q::c(&TestBuilder_TestModel::id) == 1 & orm::q::c(&TestBuilder_TestModel::name) < "John"
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_update_query_Full)
{
	auto expected = R"(UPDATE "test" SET "test"."name" = 'Hello' WHERE "test"."id" = 1;)";
	auto actual = this->sql_builder.sql_update(
		TestBuilder_TestModel::meta_table_name,
		R"("test"."name" = 'Hello')",
		orm::q::Condition(R"("test"."id" = 1)")
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_update_query_WithoutCondition)
{
	auto expected = R"(UPDATE "test" SET "test"."name" = 'Hello';)";
	auto actual = this->sql_builder.sql_update(
		TestBuilder_TestModel::meta_table_name, R"("test"."name" = 'Hello')", {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_update_query_ThrowsEmptyTableName)
{
	ASSERT_THROW(
		auto _ = this->sql_builder.sql_update("", R"("test"."name" = 'Hello')", {}),
		orm::QueryError
	);
}

TEST_F(DefaultSQLBuilder_TestCase, make_update_query_ThrowsEmptyColumns)
{
	ASSERT_THROW(
		auto _ = this->sql_builder.sql_update(TestBuilder_TestModel::meta_table_name, "", {}),
		orm::QueryError
	);
}

TEST_F(DefaultSQLBuilder_TestCase, make_delete_query_Full)
{
	auto expected = R"(DELETE FROM "test" WHERE "test"."id" = 1;)";
	auto actual = this->sql_builder.sql_delete(
		TestBuilder_TestModel::meta_table_name,
		orm::q::Condition(R"("test"."id" = 1)")
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_delete_query_WithoutCondition)
{
	auto expected = R"(DELETE FROM "test";)";
	auto actual = this->sql_builder.sql_delete(
		TestBuilder_TestModel::meta_table_name, {}
	);
	ASSERT_EQ(expected, actual);
}

TEST_F(DefaultSQLBuilder_TestCase, make_delete_query_ThrowsEmptyTableName)
{
	ASSERT_THROW(auto _ = this->sql_builder.sql_delete("", {}), orm::QueryError);
}
