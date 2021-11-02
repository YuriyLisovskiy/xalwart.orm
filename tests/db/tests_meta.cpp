/**
 * tests_meta.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/db/meta.h"
#include "../../src/db/model.h"

using namespace xw;

class TestCase_Model_meta : public ::testing::Test
{
protected:
	class TestModel : public orm::db::Model
	{
	public:
		int id{};
		double non_existent_column{};

		static constexpr const char* meta_table_name = "test_models";

		inline static const std::tuple meta_columns = {
			orm::db::make_pk_column_meta("id", &TestCase_Model_meta::TestModel::id)
		};

		inline void __orm_set_column__(const std::string& column_name, const char* data) override
		{
			this->__orm_set_column_data__(TestModel::meta_columns, column_name, data);
		}
	};
};

TEST_F(TestCase_Model_meta, get_table_name)
{
	ASSERT_EQ(orm::db::get_table_name<TestCase_Model_meta::TestModel>(), "test_models");
}

TEST_F(TestCase_Model_meta, get_pk_name)
{
	ASSERT_EQ(orm::db::get_pk_name<TestCase_Model_meta::TestModel>(), "id");
}

TEST_F(TestCase_Model_meta, make_fk_CutEndingS)
{
	auto expected = "test_model_id";
	ASSERT_EQ(orm::db::make_fk<TestCase_Model_meta::TestModel>(), expected);
}

TEST_F(TestCase_Model_meta, get_column_name_FoundColumn)
{
	std::string expected = "id";
	ASSERT_EQ(orm::db::get_column_name(&TestCase_Model_meta::TestModel::id), expected);
}

TEST_F(TestCase_Model_meta, get_column_name_ThrowsColumnNotFound)
{
	ASSERT_THROW(
		orm::db::get_column_name(&TestCase_Model_meta::TestModel::non_existent_column),
		ValueError
	);
}

class TestCase_meta_TestM : public orm::db::Model
{
public:
	int custom_identifier{};

	static constexpr const char* meta_table_name = "test";
	inline static const std::tuple meta_columns = {
		orm::db::make_pk_column_meta("custom_identifier", &TestCase_meta_TestM::custom_identifier)
	};

	inline void __orm_set_column__(const std::string& column_name, const char* data) override
	{
		this->__orm_set_column_data__(TestCase_meta_TestM::meta_columns, column_name, data);
	}
};

TEST(TestCase_utility, make_fk)
{
	auto expected = "test_custom_identifier";
	ASSERT_EQ(orm::db::make_fk<TestCase_meta_TestM>(), expected);
}

class TestModelWithoutPk : public orm::db::Model
{
public:
	static constexpr const char* meta_table_name = "test_models_without_pk";
	inline static std::tuple<> meta_columns;
};

TEST(TestCase_meta, get_pk_name_Empty)
{
	ASSERT_EQ(orm::db::get_pk_name<TestModelWithoutPk>(), "");
}

TEST(TestCase_meta, make_fk_ThrowsPkRequired)
{
	ASSERT_THROW(orm::db::make_fk<TestModelWithoutPk>(), orm::QueryError);
}
