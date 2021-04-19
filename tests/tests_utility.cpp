/**
 * tests_utility.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../src/utility.h"
#include "../src/model.h"

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
	ASSERT_EQ(orm::util::as<float>("128.0f"), 128.0f);
}

TEST(TestCase_utility, as_double)
{
	ASSERT_EQ(orm::util::as<double>("87654345.0"), 87654345.0);
}

TEST(TestCase_utility, as_long_double)
{
	ASSERT_EQ(orm::util::as<long double>("45733"), (long double)45733);
}

TEST(TestCase_utility, as_std_string)
{
	ASSERT_EQ(orm::util::as<std::string>("Hello, World"), std::string("Hello, World"));
}

TEST(TestCase_utility, as_std_c_string)
{
	ASSERT_EQ(orm::util::as<const char*>("Hello, World"), "Hello, World");
}

TEST(TestCase_utility, quote_str_AlreadyQuoted)
{
	ASSERT_EQ(orm::util::quote_str(R"("Hello")"), R"("Hello")");
}

TEST(TestCase_utility, quote_str_NotQuoted)
{
	ASSERT_EQ(orm::util::quote_str("Hello"), R"("Hello")");
}

TEST(TestCase_utility, compare_any_True)
{
	int a = 10, b = 10;
	ASSERT_TRUE(orm::util::compare_any(a, b));
}

TEST(TestCase_utility, compare_any_FalseWithSameType)
{
	int a = 10, b = 7;
	ASSERT_FALSE(orm::util::compare_any(a, b));
}

TEST(TestCase_utility, compare_any_FalseWithDifferentTypes)
{
	int a = 10;
	std::string s = "Hello";
	ASSERT_FALSE(orm::util::compare_any(a, s));
}

class TestM : public orm::Model
{
public:
	int custom_identifier{};

	static constexpr const char* meta_table_name = "test";
	inline static const std::tuple meta_columns = {
		orm::make_pk_column_meta("custom_identifier", &TestM::custom_identifier)
	};

	inline void __set_attr__(const char* attr_name, const void* data) override
	{
		this->set_attribute_for<TestM>(TestM::meta_columns, attr_name, data);
	}

	[[nodiscard]]
	inline std::shared_ptr<const Object> __get_attr__(const char* attr_name) const override
	{
		return this->get_attribute_from<TestM>(TestM::meta_columns, attr_name);
	}
};

class TestModelWithoutPk : public orm::Model
{
public:
	static constexpr const char* meta_table_name = "test_models_without_pk";
	inline static std::tuple<> meta_columns;
};

TEST(TestCase_utility, check_model_Success)
{
	ASSERT_NO_THROW(orm::util::check_model<TestM>());
}

TEST(TestCase_utility, check_model_ThrowsPkNotFound)
{
	ASSERT_THROW(orm::util::check_model<TestModelWithoutPk>(), orm::ModelError);
}

class MultiPkModel : public orm::Model
{
public:
	int id{};
	int another_id{};

	static constexpr const char* meta_table_name = "test";
	inline static const std::tuple meta_columns = {
		orm::make_pk_column_meta("id", &MultiPkModel::id),
		orm::make_pk_column_meta("another_id", &MultiPkModel::another_id)
	};

	inline void __set_attr__(const char* attr_name, const void* data) override
	{
		this->set_attribute_for<MultiPkModel>(MultiPkModel::meta_columns, attr_name, data);
	}

	[[nodiscard]]
	inline std::shared_ptr<const Object> __get_attr__(const char* attr_name) const override
	{
		return this->get_attribute_from<MultiPkModel>(MultiPkModel::meta_columns, attr_name);
	}
};

TEST(TestCase_utility, check_model_ThrowsMultiplePksFound)
{
	ASSERT_THROW(orm::util::check_model<MultiPkModel>(), orm::ModelError);
}
