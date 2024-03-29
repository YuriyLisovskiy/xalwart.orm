/**
 * tests_utility.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../src/utility.h"
#include "../src/db/model.h"

using namespace xw;


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

class TestM : public orm::db::Model
{
public:
	int custom_identifier{};

	static constexpr const char* meta_table_name = "test";
	inline static const std::tuple meta_columns = {
		orm::db::make_pk_column_meta("custom_identifier", &TestM::custom_identifier)
	};

	inline void __orm_set_column__(const std::string& column_name, const char* data) override
	{
		this->__orm_set_column_data__(TestM::meta_columns, column_name, data);
	}
};

class TestModelWithoutPk : public orm::db::Model
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

class MultiPkModel : public orm::db::Model
{
public:
	int id{};
	int another_id{};

	static constexpr const char* meta_table_name = "test";
	inline static const std::tuple meta_columns = {
		orm::db::make_pk_column_meta("id", &MultiPkModel::id),
		orm::db::make_pk_column_meta("another_id", &MultiPkModel::another_id)
	};

	inline void __orm_set_column__(const std::string& column_name, const char* data) override
	{
		this->__orm_set_column_data__(MultiPkModel::meta_columns, column_name, data);
	}
};

TEST(TestCase_utility, check_model_ThrowsMultiplePksFound)
{
	ASSERT_THROW(orm::util::check_model<MultiPkModel>(), orm::ModelError);
}
