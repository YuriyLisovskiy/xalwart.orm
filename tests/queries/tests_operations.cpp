/**
 * queries/tests_operations.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/operations.h"

using namespace xw;

class TestModel : public orm::Model
{
public:
	static constexpr const char* meta_table_name = "test_model";

	long id{};
	std::string name;
};

TEST(TestCase_Q_ordering, ascending)
{
	std::string expected = "\"id\" ASC";
	auto actual = orm::q::ascending("id");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Q_ordering, descending)
{
	std::string expected = "\"id\" DESC";
	auto actual = orm::q::descending("id");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Q_equals, fundamental)
{
	std::string expected = R"("test_model"."id" = 1)";
	auto actual = (std::string)(orm::q::c<TestModel>("id") == 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_equals, string)
{
	std::string expected = R"("test_model"."name" = 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") == std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_equals, c_string)
{
	std::string expected = R"("test_model"."name" = 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") == "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_not_equals, fundamental)
{
	std::string expected = R"("test_model"."id" != 1)";
	auto actual = (std::string)(orm::q::c<TestModel>("id") != 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_not_equals, string)
{
	std::string expected = R"("test_model"."name" != 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") != std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_not_equals, c_string)
{
	std::string expected = R"("test_model"."name" != 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") != "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_less, fundamental)
{
	std::string expected = R"("test_model"."id" < 1)";
	auto actual = (std::string)(orm::q::c<TestModel>("id") < 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_less, string)
{
	std::string expected = R"("test_model"."name" < 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") < std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_less, c_string)
{
	std::string expected = R"("test_model"."name" < 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") < "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_greater, fundamental)
{
	std::string expected = R"("test_model"."id" > 1)";
	auto actual = (std::string)(orm::q::c<TestModel>("id") > 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_greater, string)
{
	std::string expected = R"("test_model"."name" > 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") > std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_greater, c_string)
{
	std::string expected = R"("test_model"."name" > 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") > "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_less_or_equals, fundamental)
{
	std::string expected = R"("test_model"."id" <= 1)";
	auto actual = (std::string)(orm::q::c<TestModel>("id") <= 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_less_or_equals, string)
{
	std::string expected = R"("test_model"."name" <= 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") <= std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_less_or_equals, c_string)
{
	std::string expected = R"("test_model"."name" <= 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") <= "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_greater_or_equals, fundamental)
{
	std::string expected = R"("test_model"."id" >= 1)";
	auto actual = (std::string)(orm::q::c<TestModel>("id") >= 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_greater_or_equals, string)
{
	std::string expected = R"("test_model"."name" >= 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") >= std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_greater_or_equals, c_string)
{
	std::string expected = R"("test_model"."name" >= 'John')";
	auto actual = (std::string)(orm::q::c<TestModel>("name") >= "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_logical, operator_and)
{
	std::string expected = R"(("id" = 1 AND "name" = 'John'))";
	auto actual = orm::q::condition("\"id\" = 1") & orm::q::condition("\"name\" = 'John'");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Q_logical, operator_or)
{
	std::string expected = R"(("id" = 1 OR "name" = 'John'))";
	auto actual = orm::q::condition("\"id\" = 1") | orm::q::condition("\"name\" = 'John'");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Q_logical, operator_not)
{
	std::string expected = "NOT (\"id\" = 1)";
	auto actual = ~orm::q::condition("\"id\" = 1");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Q_between, fundamental)
{
	std::string expected = R"("test_model"."id" BETWEEN 1 AND 5)";
	auto actual = orm::q::between<TestModel, int>("id", 1, 5);
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Q_between, string)
{
	std::string expected = R"("test_model"."name" BETWEEN 'John' AND 'Steve')";
	auto actual = orm::q::between<TestModel, std::string>("name", "John", "Steve");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Q_between, c_string)
{
	std::string expected = R"("test_model"."name" BETWEEN 'John' AND 'Steve')";
	auto actual = orm::q::between<TestModel, const char*>("name", "John", "Steve");
	ASSERT_EQ(expected, (std::string)actual);
}
