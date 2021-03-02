/**
 * queries/tests_operations.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/operations.h"

using namespace xw;

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
	std::string expected = "\"id\" = 1";
	auto actual = (std::string)orm::q::equals("id", 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_equals, string)
{
	std::string expected = "\"name\" = 'John'";
	auto actual = (std::string)orm::q::equals("name", std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_equals, c_string)
{
	std::string expected = "\"name\" = 'John'";
	auto actual = (std::string)orm::q::equals("name", "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_not_equals, fundamental)
{
	std::string expected = "\"id\" != 1";
	auto actual = (std::string)orm::q::not_equals("id", 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_not_equals, string)
{
	std::string expected = "\"name\" != 'John'";
	auto actual = (std::string)orm::q::not_equals("name", std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_not_equals, c_string)
{
	std::string expected = "\"name\" != 'John'";
	auto actual = (std::string)orm::q::not_equals("name", "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_less, fundamental)
{
	std::string expected = "\"id\" < 1";
	auto actual = (std::string)orm::q::less("id", 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_less, string)
{
	std::string expected = "\"name\" < 'John'";
	auto actual = (std::string)orm::q::less("name", std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_less, c_string)
{
	std::string expected = "\"name\" < 'John'";
	auto actual = (std::string)orm::q::less("name", "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_greater, fundamental)
{
	std::string expected = "\"id\" > 1";
	auto actual = (std::string)orm::q::greater("id", 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_greater, string)
{
	std::string expected = "\"name\" > 'John'";
	auto actual = (std::string)orm::q::greater("name", std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_greater, c_string)
{
	std::string expected = "\"name\" > 'John'";
	auto actual = (std::string)orm::q::greater("name", "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_less_or_equals, fundamental)
{
	std::string expected = "\"id\" <= 1";
	auto actual = (std::string)orm::q::less_or_equals("id", 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_less_or_equals, string)
{
	std::string expected = "\"name\" <= 'John'";
	auto actual = (std::string)orm::q::less_or_equals("name", std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_less_or_equals, c_string)
{
	std::string expected = "\"name\" <= 'John'";
	auto actual = (std::string)orm::q::less_or_equals("name", "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_greater_or_equals, fundamental)
{
	std::string expected = "\"id\" >= 1";
	auto actual = (std::string)orm::q::greater_or_equals("id", 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_greater_or_equals, string)
{
	std::string expected = "\"name\" >= 'John'";
	auto actual = (std::string)orm::q::greater_or_equals("name", std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_greater_or_equals, c_string)
{
	std::string expected = "\"name\" >= 'John'";
	auto actual = (std::string)orm::q::greater_or_equals("name", "John");
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
	auto actual = !orm::q::condition("\"id\" = 1");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Q_between, fundamental)
{
	std::string expected = "\"id\" BETWEEN 1 AND 5";
	auto actual = orm::q::between("id", 1, 5);
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Q_between, string)
{
	std::string expected = "\"name\" BETWEEN 'John' AND 'Steve'";
	auto actual = orm::q::between<std::string>("name", "John", "Steve");
	ASSERT_EQ(expected, (std::string)actual);
}

TEST(TestCase_Q_between, c_string)
{
	std::string expected = "\"name\" BETWEEN 'John' AND 'Steve'";
	auto actual = orm::q::between<const char*>("name", "John", "Steve");
	ASSERT_EQ(expected, (std::string)actual);
}
