/**
 * tests_utility.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../src/model.h"
#include "../src/utility.h"

using namespace xw;

TEST(TestCase_utility, as_short_int)
{
	ASSERT_EQ(orm::util::as<short int>("-32766"), -32766);
}

TEST(TestCase_utility, as_unsigned_short_int)
{
	ASSERT_EQ(orm::util::as<unsigned short int>("65530"), 65530);
}

TEST(TestCase_utility, as_unsigned_int)
{
	ASSERT_EQ(orm::util::as<unsigned int>("4294967"), 4294967);
}

TEST(TestCase_utility, as_int)
{
	ASSERT_EQ(orm::util::as<int>("-21474836"), -21474836);
}

TEST(TestCase_utility, as_long_int)
{
	ASSERT_EQ(orm::util::as<long int>("2147483647"), 2147483647);
}

TEST(TestCase_utility, as_unsigned_long_int)
{
	ASSERT_EQ(orm::util::as<unsigned long int>("4294967293"), 4294967293);
}

TEST(TestCase_utility, as_long_long_int)
{
	ASSERT_EQ(orm::util::as<long long int>("4294972934294967293"), 4294972934294967293);
}

TEST(TestCase_utility, as_unsigned_long_long_int)
{
	ASSERT_EQ(orm::util::as<unsigned long long int>("1844674407370955161"), 1844674407370955161);
}

TEST(TestCase_utility, as_float)
{
	ASSERT_EQ(orm::util::as<float>("128.8765f"), 128.8765f);
}

TEST(TestCase_utility, as_double)
{
	ASSERT_EQ(orm::util::as<double>("87654345.3456789"), 87654345.3456789);
}

TEST(TestCase_utility, as_long_double)
{
	ASSERT_EQ(orm::util::as<long double>("45733"), (long double)45733);
}

TEST(TestCase_utility, as_std_string)
{
	ASSERT_EQ(orm::util::as<std::string>("Hello, World"), std::string("Hello, World"));
}

TEST(TestCase_utility, quote_str_AlreadyQuoted)
{
	ASSERT_EQ(orm::util::quote_str(R"("Hello")"), R"("Hello")");
}

TEST(TestCase_utility, quote_str_NotQuoted)
{
	ASSERT_EQ(orm::util::quote_str("Hello"), R"("Hello")");
}

class Model_TestCase : public ::testing::Test
{
protected:
	class TestModel : public orm::Model<TestModel>
	{
	public:
		int id{};

		static constexpr const char* meta_table_name = "test_models";

		static const std::tuple<orm::column_meta_t<TestModel, int>> meta_columns;
	};
};

const std::tuple<orm::column_meta_t<Model_TestCase::TestModel, int>> Model_TestCase::TestModel::meta_columns = {
	orm::make_pk_column_meta("id", &Model_TestCase::TestModel::id)
};

TEST_F(Model_TestCase, get_table_name)
{
	ASSERT_EQ(orm::util::get_table_name<Model_TestCase::TestModel>(), "test_models");
}

TEST_F(Model_TestCase, get_pk_name)
{
	ASSERT_EQ(orm::util::get_pk_name<Model_TestCase::TestModel>(), "id");
}

TEST_F(Model_TestCase, make_fk_CutEndingS)
{
	auto expected = "test_model_id";
	ASSERT_EQ(orm::util::make_fk<Model_TestCase::TestModel>(), expected);
}

class TestM : public orm::Model<TestM>
{
public:
	int custom_identifier{};

	static constexpr const char* meta_table_name = "test";
	static const std::tuple<orm::column_meta_t<TestM, int>> meta_columns;
};

const std::tuple<orm::column_meta_t<TestM, int>> TestM::meta_columns = {
	orm::make_pk_column_meta("custom_identifier", &TestM::custom_identifier)
};

TEST(ModelUnitlity_TestCase, make_fk)
{
	auto expected = "test_custom_identifier";
	ASSERT_EQ(orm::util::make_fk<TestM>(), expected);
}
