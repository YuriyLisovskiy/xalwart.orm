/**
 * queries/tests_operations.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/conditions.h"

using namespace xw;

class TestModel : public orm::db::Model
{
public:
	static constexpr const char* meta_table_name = "test_model";

	long id{};
	std::string name;
	const char* name_c_str;

	inline static const std::tuple meta_columns = {
		orm::db::make_pk_column_meta("id", &TestModel::id),
		orm::db::make_column_meta("name", &TestModel::name),
		orm::db::make_column_meta("name_c_str", &TestModel::name_c_str)
	};

	inline void __orm_set_column__(const std::string& column_name, const char* data) override
	{
		this->__orm_set_column_data__(TestModel::meta_columns, column_name, data);
	}
};

TEST(TestCase_Conditions, Ordering_Ascending)
{
	auto expected = R"("test_model"."id" ASC)";
	auto actual = orm::q::asc(&TestModel::id);
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, Ordering_Descending)
{
	auto expected = R"("test_model"."id" DESC)";
	auto actual = orm::q::desc(&TestModel::id);
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, equals_Fundamental)
{
	std::string expected = R"("test_model"."id" = 1)";
	auto actual = (std::string)(orm::q::c(&TestModel::id) == 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, equals_String)
{
	std::string expected = R"("test_model"."name" = 'John')";
	auto actual = (std::string)(orm::q::c(&TestModel::name) == std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, not_equals_Fundamental)
{
	std::string expected = R"("test_model"."id" != 1)";
	auto actual = (std::string)(orm::q::c(&TestModel::id) != 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, not_equals_String)
{
	std::string expected = R"("test_model"."name" != 'John')";
	auto actual = (std::string)(orm::q::c(&TestModel::name) != std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, less_Fundamental)
{
	std::string expected = R"("test_model"."id" < 1)";
	auto actual = (std::string)(orm::q::c(&TestModel::id) < 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, less_String)
{
	std::string expected = R"("test_model"."name" < 'John')";
	auto actual = (std::string)(orm::q::c(&TestModel::name) < "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, greater_Fundamental)
{
	std::string expected = R"("test_model"."id" > 1)";
	auto actual = (std::string)(orm::q::c(&TestModel::id) > 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, greater_String)
{
	std::string expected = R"("test_model"."name" > 'John')";
	auto actual = (std::string)(orm::q::c(&TestModel::name) > "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, less_or_equals_Fundamental)
{
	std::string expected = R"("test_model"."id" <= 1)";
	auto actual = (std::string)(orm::q::c(&TestModel::id) <= 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, less_or_equals_String)
{
	std::string expected = R"("test_model"."name" <= 'John')";
	auto actual = (std::string)(orm::q::c(&TestModel::name) <= "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, greater_or_equals_Fundamental)
{
	std::string expected = R"("test_model"."id" >= 1)";
	auto actual = (std::string)(orm::q::c(&TestModel::id) >= 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, greater_or_equals_String)
{
	std::string expected = R"("test_model"."name" >= 'John')";
	auto actual = (std::string)(orm::q::c(&TestModel::name) >= "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, is_null_Id)
{
	auto expected = R"("test_model"."id" IS NULL)";
	ASSERT_EQ((std::string)orm::q::is_null(&TestModel::id), expected);
}

TEST(TestCase_Conditions, is_not_null_Id)
{
	auto expected = R"("test_model"."id" IS NOT NULL)";
	ASSERT_EQ((std::string)orm::q::is_not_null(&TestModel::id), expected);
}

TEST(TestCase_Conditions, LogicalOperatorAnd)
{
	std::string expected = R"(("id" = 1 AND "name" = 'John'))";
	auto actual = orm::q::Condition("\"id\" = 1") & orm::q::Condition("\"name\" = 'John'");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, LogicalOperatorOr)
{
	std::string expected = R"(("id" = 1 OR "name" = 'John'))";
	auto actual = orm::q::Condition("\"id\" = 1") | orm::q::Condition("\"name\" = 'John'");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, LogicalOperatorNot)
{
	std::string expected = "NOT (\"id\" = 1)";
	auto actual = ~orm::q::Condition("\"id\" = 1");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, between_Fundamental)
{
	std::string expected = R"("test_model"."id" BETWEEN 1 AND 5)";
	auto actual = orm::q::between<long>(&TestModel::id, 1, 5);
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, between_String)
{
	std::string expected = R"("test_model"."name" BETWEEN 'John' AND 'Steve')";
	auto actual = orm::q::between(&TestModel::name, "John", "Steve");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, between_CString)
{
	std::string expected = R"("test_model"."name_c_str" BETWEEN 'John' AND 'Steve')";
	auto actual = orm::q::between(&TestModel::name_c_str, "John", "Steve");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, like_Default)
{
	auto expected = R"("test_model"."id" LIKE '%Wild%')";
	ASSERT_EQ((std::string)orm::q::like(&TestModel::id, "%Wild%"), expected);
}

TEST(TestCase_Conditions, like_WithEscape)
{
	auto expected = R"("test_model"."id" LIKE '%Wild%' ESCAPE '\')";
	ASSERT_EQ((std::string)orm::q::like(&TestModel::id, "%Wild%", "\\"), expected);
}

TEST(TestCase_Conditions, in_Fundamental)
{
	auto expected = R"("test_model"."id" IN (10, 11, 12))";
	auto range = {10, 11, 12};
	auto cond = orm::q::in(&TestModel::id, range.begin(), range.end());
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, in_String)
{
	auto expected = R"("test_model"."name" IN ('John', 'Steve'))";
	std::vector<std::string> range = {"John", "Steve"};
	auto cond = orm::q::in(&TestModel::name, range.begin(), range.end());
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, in_CString)
{
	auto expected = R"("test_model"."name_c_str" IN ('John', 'Steve'))";
	auto range = {"John", "Steve"};
	auto cond = orm::q::in(&TestModel::name_c_str, range.begin(), range.end());
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, in_FundamentalInitializerList)
{
	auto expected = R"("test_model"."id" IN (10, 11, 12))";
	auto cond = orm::q::in(&TestModel::id, {10, 11, 12});
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, in_StringInitializerList)
{
	auto expected = R"("test_model"."name" IN ('John', 'Steve'))";
	auto cond = orm::q::in(&TestModel::name, std::initializer_list<std::string>{"John", "Steve"});
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, in_CStringInitializerList)
{
	auto expected = R"("test_model"."name_c_str" IN ('John', 'Steve'))";
	auto cond = orm::q::in(&TestModel::name_c_str, {"John", "Steve"});
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, in_Fundamental_ThrowsEmptyRange)
{
	std::initializer_list<int> range = {};
	ASSERT_THROW(
		orm::q::in(&TestModel::id, range.begin(), range.end()),
		orm::QueryError
	);
}

TEST(TestCase_Conditions, in_String_ThrowsEmptyRange)
{
	std::vector<std::string> range = {};
	ASSERT_THROW(
		orm::q::in(&TestModel::name, range.begin(), range.end()),
		orm::QueryError
	);
}

TEST(TestCase_Conditions, in_CString_ThrowsEmptyRange)
{
	std::initializer_list<const char*> range = {};
	ASSERT_THROW(
		orm::q::in(&TestModel::name_c_str, range.begin(), range.end()),
		orm::QueryError
	);
}

TEST(TestCase_Conditions, in_FundamentalInitializerList_ThrowsEmptyRange)
{
	ASSERT_THROW(
		orm::q::in(&TestModel::id, std::initializer_list<int>{}),
		orm::QueryError
	);
}

TEST(TestCase_Conditions, in_StringInitializerList_ThrowsEmptyRange)
{
	ASSERT_THROW(
		orm::q::in(&TestModel::name, std::initializer_list<std::string>{}),
		orm::QueryError
	);
}

TEST(TestCase_Conditions, in_CStringInitializerList_ThrowsEmptyRange)
{
	ASSERT_THROW(
		orm::q::in(&TestModel::name_c_str, std::initializer_list<const char*>{}),
		orm::QueryError
	);
}

class OtherTestModel : public orm::db::Model
{
public:
	static constexpr const char* meta_table_name = "other_test_models";

	long id{};
	std::string name;

	inline void __orm_set_column__(const std::string& column_name, const char* data) override
	{
		this->__orm_set_column_data__(OtherTestModel::meta_columns, column_name, data);
	}
};

TEST(TestCase_Conditions, join_on)
{
	auto expected = R"(CUSTOM JOIN "other_test_models" ON "test_model"."id" = "other_test_models"."test_model_id")";
	auto cond = orm::q::join_on<TestModel, OtherTestModel>("CUSTOM", "test_model_id");
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, inner_join_on)
{
	auto expected = R"(INNER JOIN "other_test_models" ON "test_model"."id" = "other_test_models"."test_model_id")";
	auto cond = orm::q::inner_on<TestModel, OtherTestModel>("test_model_id");
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, left_join_on)
{
	auto expected = R"(LEFT JOIN "other_test_models" ON "test_model"."id" = "other_test_models"."test_model_id")";
	auto cond = orm::q::left_on<TestModel, OtherTestModel>("test_model_id");
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, cross_join_on)
{
	auto expected = R"(CROSS JOIN "other_test_models" ON "test_model"."id" = "other_test_models"."test_model_id")";
	auto cond = orm::q::cross_on<TestModel, OtherTestModel>("test_model_id");
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, join_on_WithCustomCondition)
{
	auto expected = R"(CUSTOM JOIN "other_test_models" ON "test_model"."id" = "other_test_models"."test_model_id" AND ("test_model"."id" = 1))";
	auto cond = orm::q::join_on<TestModel, OtherTestModel>("CUSTOM", "test_model_id", orm::q::c(&TestModel::id) == 1);
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, inner_join_on_WithCustomCondition)
{
	auto expected = R"(INNER JOIN "other_test_models" ON "test_model"."id" = "other_test_models"."test_model_id" AND ("test_model"."id" = 1))";
	auto cond = orm::q::inner_on<TestModel, OtherTestModel>("test_model_id", orm::q::c(&TestModel::id) == 1);
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, left_join_on_WithCustomCondition)
{
	auto expected = R"(LEFT JOIN "other_test_models" ON "test_model"."id" = "other_test_models"."test_model_id" AND ("test_model"."id" = 1))";
	auto cond = orm::q::left_on<TestModel, OtherTestModel>("test_model_id", orm::q::c(&TestModel::id) == 1);
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, cross_join_on_WithCustomCondition)
{
	auto expected = R"(CROSS JOIN "other_test_models" ON "test_model"."id" = "other_test_models"."test_model_id" AND ("test_model"."id" = 1))";
	auto cond = orm::q::cross_on<TestModel, OtherTestModel>("test_model_id", orm::q::c(&TestModel::id) == 1);
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, join_on_WithAutoGeneratedFk)
{
	auto expected = R"(CUSTOM JOIN "other_test_models" ON "test_model"."id" = "other_test_models"."test_model_id")";
	auto cond = orm::q::join_on<TestModel, OtherTestModel>("CUSTOM");
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, inner_join_on_WithAutoGeneratedFk)
{
	auto expected = R"(INNER JOIN "other_test_models" ON "test_model"."id" = "other_test_models"."test_model_id")";
	auto cond = orm::q::inner_on<TestModel, OtherTestModel>();
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, left_join_on_WithAutoGeneratedFk)
{
	auto expected = R"(LEFT JOIN "other_test_models" ON "test_model"."id" = "other_test_models"."test_model_id")";
	auto cond = orm::q::left_on<TestModel, OtherTestModel>();
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, cross_join_on_WithAutoGeneratedFk)
{
	auto expected = R"(CROSS JOIN "other_test_models" ON "test_model"."id" = "other_test_models"."test_model_id")";
	auto cond = orm::q::cross_on<TestModel, OtherTestModel>();
	ASSERT_EQ((std::string)cond, expected);
}
