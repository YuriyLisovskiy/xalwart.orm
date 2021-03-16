/**
 * queries/tests_operations.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/conditions.h"

using namespace xw;

class TestModel : public orm::Model
{
public:
	static constexpr const char* meta_table_name = "test_model";

	long id{};
	std::string name;
};

TEST(TestCase_Conditions, ordering_Ascending)
{
	std::string expected = "\"id\" ASC";
	auto actual = orm::q::ascending("id");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, ordering_Descending)
{
	std::string expected = "\"id\" DESC";
	auto actual = orm::q::descending("id");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, equals_Fundamental)
{
	std::string expected = R"("test_model"."id" = 1)";
	auto actual = (std::string)(orm::q::c<TestModel>("id") == 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, equals_String)
{
	std::string expected = R"("test_model"."name" = 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") == std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, equals_CString)
{
	std::string expected = R"("test_model"."name" = 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") == "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, not_equals_Fundamental)
{
	std::string expected = R"("test_model"."id" != 1)";
	auto actual = (std::string)(orm::q::c<TestModel>("id") != 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, not_equals_String)
{
	std::string expected = R"("test_model"."name" != 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") != std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, not_equals_CString)
{
	std::string expected = R"("test_model"."name" != 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") != "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, less_Fundamental)
{
	std::string expected = R"("test_model"."id" < 1)";
	auto actual = (std::string)(orm::q::c<TestModel>("id") < 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, less_String)
{
	std::string expected = R"("test_model"."name" < 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") < std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, less_CString)
{
	std::string expected = R"("test_model"."name" < 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") < "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, greater_Fundamental)
{
	std::string expected = R"("test_model"."id" > 1)";
	auto actual = (std::string)(orm::q::c<TestModel>("id") > 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, greater_String)
{
	std::string expected = R"("test_model"."name" > 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") > std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, greater_CString)
{
	std::string expected = R"("test_model"."name" > 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") > "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, less_or_equals_Fundamental)
{
	std::string expected = R"("test_model"."id" <= 1)";
	auto actual = (std::string)(orm::q::c<TestModel>("id") <= 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, less_or_equals_String)
{
	std::string expected = R"("test_model"."name" <= 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") <= std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, less_or_equals_CString)
{
	std::string expected = R"("test_model"."name" <= 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") <= "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, greater_or_equals_Fundamental)
{
	std::string expected = R"("test_model"."id" >= 1)";
	auto actual = (std::string)(orm::q::c<TestModel>("id") >= 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, greater_or_equals_String)
{
	std::string expected = R"("test_model"."name" >= 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") >= std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, greater_or_equals_CString)
{
	std::string expected = R"("test_model"."name" >= 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") >= "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Conditions, is_null_Id)
{
	auto expected = R"("test_model"."id" IS NULL)";
	ASSERT_EQ((std::string)orm::q::is_null<TestModel>("id"), expected);
}

TEST(TestCase_Conditions, is_not_null_Id)
{
	auto expected = R"("test_model"."id" IS NOT NULL)";
	ASSERT_EQ((std::string)orm::q::is_not_null<TestModel>("id"), expected);
}

TEST(TestCase_Conditions, LogicalOperatorAnd)
{
	std::string expected = R"(("id" = 1 AND "name" = 'John'))";
	auto actual = orm::q::condition_t("\"id\" = 1") & orm::q::condition_t("\"name\" = 'John'");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, LogicalOperatorOr)
{
	std::string expected = R"(("id" = 1 OR "name" = 'John'))";
	auto actual = orm::q::condition_t("\"id\" = 1") | orm::q::condition_t("\"name\" = 'John'");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, LogicalOperatorNot)
{
	std::string expected = "NOT (\"id\" = 1)";
	auto actual = ~orm::q::condition_t("\"id\" = 1");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, between_Fundamental)
{
	std::string expected = R"("test_model"."id" BETWEEN 1 AND 5)";
	auto actual = orm::q::between<TestModel, int>("id", 1, 5);
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, between_String)
{
	std::string expected = R"("test_model"."name" BETWEEN 'John' AND 'Steve')";
	auto actual = orm::q::between<TestModel>(
		"name", std::string("John"), std::string("Steve")
	);
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, between_CString)
{
	std::string expected = R"("test_model"."name" BETWEEN 'John' AND 'Steve')";
	auto actual = orm::q::between<TestModel>("name", "John", "Steve");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Conditions, like_Default)
{
	auto expected = R"("test_model"."id" LIKE '%Wild%')";
	ASSERT_EQ((std::string)orm::q::like<TestModel>("id", "%Wild%"), expected);
}

TEST(TestCase_Conditions, like_WithEscape)
{
	auto expected = R"("test_model"."id" LIKE '%Wild%' ESCAPE '\')";
	ASSERT_EQ((std::string)orm::q::like<TestModel>("id", "%Wild%", "\\"), expected);
}

TEST(TestCase_Conditions, in_Fundamental)
{
	auto expected = R"("test_model"."id" IN (10, 11, 12))";
	auto range = {10, 11, 12};
	auto cond = orm::q::in<TestModel>("id", range.begin(), range.end());
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, in_String)
{
	auto expected = R"("test_model"."name" IN ('John', 'Steve'))";
	std::vector<std::string> range = {"John", "Steve"};
	auto cond = orm::q::in<TestModel>("name", range.begin(), range.end());
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, in_CString)
{
	auto expected = R"("test_model"."name" IN ('John', 'Steve'))";
	auto range = {"John", "Steve"};
	auto cond = orm::q::in<TestModel>("name", range.begin(), range.end());
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, in_FundamentalInitializerList)
{
	auto expected = R"("test_model"."id" IN (10, 11, 12))";
	auto cond = orm::q::in<TestModel>("id", {10, 11, 12});
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, in_StringInitializerList)
{
	auto expected = R"("test_model"."name" IN ('John', 'Steve'))";
	auto cond = orm::q::in<TestModel>("name", std::initializer_list<std::string>{"John", "Steve"});
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, in_CStringInitializerList)
{
	auto expected = R"("test_model"."name" IN ('John', 'Steve'))";
	auto cond = orm::q::in<TestModel>("name", {"John", "Steve"});
	ASSERT_EQ((std::string)cond, expected);
}

TEST(TestCase_Conditions, in_Fundamental_ThrowsEmptyRange)
{
	std::initializer_list<int> range = {};
	ASSERT_THROW(
		orm::q::in<TestModel>("id", range.begin(), range.end()),
		orm::QueryError
	);
}

TEST(TestCase_Conditions, in_String_ThrowsEmptyRange)
{
	std::vector<std::string> range = {};
	ASSERT_THROW(
		orm::q::in<TestModel>("name", range.begin(), range.end()),
		orm::QueryError
	);
}

TEST(TestCase_Conditions, in_CString_ThrowsEmptyRange)
{
	std::initializer_list<const char*> range = {};
	ASSERT_THROW(
		orm::q::in<TestModel>("name", range.begin(), range.end()),
		orm::QueryError
	);
}

TEST(TestCase_Conditions, in_FundamentalInitializerList_ThrowsEmptyRange)
{
	ASSERT_THROW(
		orm::q::in<TestModel>("id", std::initializer_list<int>{}),
		orm::QueryError
	);
}

TEST(TestCase_Conditions, in_StringInitializerList_ThrowsEmptyRange)
{
	ASSERT_THROW(
		orm::q::in<TestModel>("name", std::initializer_list<std::string>{}),
		orm::QueryError
	);
}

TEST(TestCase_Conditions, in_CStringInitializerList_ThrowsEmptyRange)
{
	ASSERT_THROW(
		orm::q::in<TestModel>("name", std::initializer_list<const char*>{}),
		orm::QueryError
	);
}
