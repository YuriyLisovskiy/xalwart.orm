/**
 * queries/tests_insert.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/insert.h"

#include "./mocked_driver.h"

using namespace xw;

struct TestCase_Q_insert_TestModel : public orm::db::Model
{
	int id{};
	std::string name;

	static constexpr const char* meta_table_name = "test_models";

	inline static const std::tuple meta_columns = {
		orm::db::make_pk_column_meta("id", &TestCase_Q_insert_TestModel::id),
		orm::db::make_column_meta("name", &TestCase_Q_insert_TestModel::name)
	};

	inline void __set_attr__(const char* attr_name, const void* data) override
	{
		this->set_attribute_to(TestCase_Q_insert_TestModel::meta_columns, attr_name, data);
	}

	[[nodiscard]]
	inline std::shared_ptr<const Object> __get_attr__(const char* attr_name) const override
	{
		return this->get_attribute_from(TestCase_Q_insert_TestModel::meta_columns, attr_name);
	}
};

class TestCase_Q_insert_One : public ::testing::Test
{
protected:
	orm::q::insert<TestCase_Q_insert_TestModel>* query;

	void SetUp() override
	{
		this->query = new orm::q::insert(TestCase_Q_insert_TestModel());
	}

	void TearDown() override
	{
		delete this->query;
	}
};

TEST_F(TestCase_Q_insert_One, commit_one_MissingDriverException)
{
	ASSERT_THROW(auto _ = this->query->commit_one(), orm::QueryError);
}

TEST_F(TestCase_Q_insert_One, commit_one_WithPkArg_MissingDriverException)
{
	int pk;
	ASSERT_THROW(this->query->commit_one(pk), orm::QueryError);
}

TEST_F(TestCase_Q_insert_One, commit_batch_MissingDriverException)
{
	ASSERT_THROW(this->query->commit_batch(), orm::QueryError);
}

TEST_F(TestCase_Q_insert_One, query_MissingDriverException)
{
	ASSERT_THROW(auto _ = this->query->query(), orm::QueryError);
}

class TestCase_Q_insert_Bulk : public ::testing::Test
{
protected:
	orm::q::insert<TestCase_Q_insert_TestModel>* query;

	void SetUp() override
	{
		this->query = new orm::q::insert(TestCase_Q_insert_TestModel());
		this->query->model(TestCase_Q_insert_TestModel());
	}

	void TearDown() override
	{
		delete this->query;
	}
};

TEST_F(TestCase_Q_insert_Bulk, commit_one_FailDueToBulkMode)
{
	ASSERT_THROW(auto _ = this->query->commit_one(), orm::QueryError);
}

TEST_F(TestCase_Q_insert_One, commit_one_WithPkArg_FailDueToBulkMode)
{
	int pk;
	ASSERT_THROW(this->query->commit_one(pk), orm::QueryError);
}

class TestCaseF_Q_insert : public ::testing::Test
{
protected:
	MockedDriver* driver;

	void SetUp() override
	{
		this->driver = new MockedDriver();
	}

	void TearDown() override
	{
		delete this->driver;
	}
};

TEST_F(TestCaseF_Q_insert, query_SingleModel)
{
	TestCase_Q_insert_TestModel model;
	model.name = "Steve";

	auto expected = R"(INSERT INTO "test_models" (name) VALUES ('Steve');)";
	auto actual = orm::q::insert(model).use(this->driver).query();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_insert, query_MultipleModels)
{
	TestCase_Q_insert_TestModel model_1;
	model_1.name = "Steve";

	TestCase_Q_insert_TestModel model_2;
	model_2.name = "John";

	auto expected = R"(INSERT INTO "test_models" (name) VALUES ('Steve'), ('John');)";
	auto actual = orm::q::insert(model_1).model(model_2).use(this->driver).query();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_insert, commit_one_ReturnedStringPk)
{
	TestCase_Q_insert_TestModel model;
	model.name = "Steve";

	ASSERT_EQ(orm::q::insert(model).use(this->driver).commit_one(), "1");
}

TEST_F(TestCaseF_Q_insert, commit_one_SetPk)
{
	TestCase_Q_insert_TestModel model;
	model.id = 0;
	model.name = "Steve";

	ASSERT_NO_THROW(orm::q::insert(model).use(this->driver).commit_one(model.id));
	ASSERT_EQ(model.id, 1);
}

TEST_F(TestCaseF_Q_insert, commit_batch_NoThrow)
{
	TestCase_Q_insert_TestModel model_1;
	model_1.name = "Steve";

	TestCase_Q_insert_TestModel model_2;
	model_2.name = "John";

	ASSERT_NO_THROW(orm::q::insert(model_1).model(model_2).use(this->driver).commit_batch());
}
