/**
 * tests_operators.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../src/q.h"

using namespace xw;


TEST(TestCase_equals, fundamental)
{
	std::string expected = "id = 1";
	auto actual = (std::string)orm::q::equals("id", 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_equals, string)
{
	std::string expected = "name = 'John'";
	auto actual = (std::string)orm::q::equals("name", std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_equals, c_string)
{
	std::string expected = "name = 'John'";
	auto actual = (std::string)orm::q::equals("name", "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_not_equals, fundamental)
{
	std::string expected = "id != 1";
	auto actual = (std::string)orm::q::not_equals("id", 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_not_equals, string)
{
	std::string expected = "name != 'John'";
	auto actual = (std::string)orm::q::not_equals("name", std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_not_equals, c_string)
{
	std::string expected = "name != 'John'";
	auto actual = (std::string)orm::q::not_equals("name", "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_less, fundamental)
{
	std::string expected = "id < 1";
	auto actual = (std::string)orm::q::less("id", 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_less, string)
{
	std::string expected = "name < 'John'";
	auto actual = (std::string)orm::q::less("name", std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_less, c_string)
{
	std::string expected = "name < 'John'";
	auto actual = (std::string)orm::q::less("name", "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_greater, fundamental)
{
	std::string expected = "id > 1";
	auto actual = (std::string)orm::q::greater("id", 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_greater, string)
{
	std::string expected = "name > 'John'";
	auto actual = (std::string)orm::q::greater("name", std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_greater, c_string)
{
	std::string expected = "name > 'John'";
	auto actual = (std::string)orm::q::greater("name", "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_less_or_equals, fundamental)
{
	std::string expected = "id <= 1";
	auto actual = (std::string)orm::q::less_or_equals("id", 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_less_or_equals, string)
{
	std::string expected = "name <= 'John'";
	auto actual = (std::string)orm::q::less_or_equals("name", std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_less_or_equals, c_string)
{
	std::string expected = "name <= 'John'";
	auto actual = (std::string)orm::q::less_or_equals("name", "John");
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_greater_or_equals, fundamental)
{
	std::string expected = "id >= 1";
	auto actual = (std::string)orm::q::greater_or_equals("id", 1);
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_greater_or_equals, string)
{
	std::string expected = "name >= 'John'";
	auto actual = (std::string)orm::q::greater_or_equals("name", std::string("John"));
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_greater_or_equals, c_string)
{
	std::string expected = "name >= 'John'";
	auto actual = (std::string)orm::q::greater_or_equals("name", "John");
	ASSERT_EQ(expected, actual);
}
