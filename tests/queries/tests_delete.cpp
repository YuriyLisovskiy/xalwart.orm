/**
 * queries/tests_delete.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/delete.h"

#include "./mocked_driver.h"

using namespace xw;

struct TestCaseF_Q_delete_TestModel : public orm::Model<TestCaseF_Q_delete_TestModel>
{
	int id{};
	std::string name;

	static constexpr const char* meta_table_name = "test_models";

	static const std::tuple<
		orm::column_meta_t<TestCaseF_Q_delete_TestModel, int>,
		orm::column_meta_t<TestCaseF_Q_delete_TestModel, std::string>
	> meta_columns;
};

const std::tuple<
	orm::column_meta_t<TestCaseF_Q_delete_TestModel, int>,
	orm::column_meta_t<TestCaseF_Q_delete_TestModel, std::string>
> TestCaseF_Q_delete_TestModel::meta_columns = {
	orm::make_pk_column_meta("id", &TestCaseF_Q_delete_TestModel::id),
	orm::make_column_meta("name", &TestCaseF_Q_delete_TestModel::name)
};

class TestCaseF_Q_delete : public ::testing::Test
{
protected:
	MockedDriver* driver;
	orm::q::delete_<TestCaseF_Q_delete_TestModel>* query;

	void SetUp() override
	{
		this->driver = new MockedDriver();

		TestCaseF_Q_delete_TestModel model;
		model.id = 0;
		model.name = "Steve";
		this->query = new orm::q::delete_(model);
	}

	void TearDown() override
	{
		delete this->driver;
		delete this->query;
	}
};

TEST_F(TestCaseF_Q_delete, commit_MissingDriverException)
{
	ASSERT_THROW(this->query->commit(), orm::QueryError);
}

TEST_F(TestCaseF_Q_delete, query_MissingDriverException)
{
	ASSERT_THROW(auto _ = this->query->query(), orm::QueryError);
}

TEST_F(TestCaseF_Q_delete, model_NoThrow)
{
	ASSERT_NO_THROW(this->query->model(TestCaseF_Q_delete_TestModel()));
}

TEST_F(TestCaseF_Q_delete, model_ThrowsNullModel)
{
	TestCaseF_Q_delete_TestModel null_model;
	null_model.mark_as_null();
	ASSERT_THROW(this->query->model(null_model), orm::QueryError);
}

TEST_F(TestCaseF_Q_delete, where_CalledMoreThanOnce)
{
	auto expected = R"(DELETE FROM "test_models" WHERE ("test_models"."id" = 1 AND "test_models"."name" LIKE '%hn');)";
	auto actual = this->query->use(this->driver)
		.where(orm::q::c(&TestCaseF_Q_delete_TestModel::id) == 1)
		.where(orm::q::like(&TestCaseF_Q_delete_TestModel::name, "%hn")).query();
	ASSERT_EQ(expected, actual);
}

TEST(TestCase_Q_delete, constructor_ThrowsNullModel)
{
	TestCaseF_Q_delete_TestModel null_model;
	null_model.mark_as_null();
	ASSERT_THROW(
		auto _ = orm::q::delete_(null_model),
		orm::QueryError
	);
}

TEST_F(TestCaseF_Q_delete, query_SingleModel)
{
	auto expected = R"(DELETE FROM "test_models" WHERE "test_models"."id" IN (0);)";
	auto actual = this->query->use(this->driver).query();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_delete, query_CustomCondition)
{
	auto expected = R"(DELETE FROM "test_models" WHERE "test_models"."name" = 'John';)";
	auto actual = this->query->use(this->driver)
		.where(orm::q::c(&TestCaseF_Q_delete_TestModel::name) == "John").query();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_delete, query_MultipleModels)
{
	auto expected = R"(DELETE FROM "test_models" WHERE "test_models"."id" IN (0, 1, 2);)";

	TestCaseF_Q_delete_TestModel model_1;
	model_1.id = 1;

	TestCaseF_Q_delete_TestModel model_2;
	model_2.id = 2;

	auto actual = this->query->use(this->driver).model(model_1).model(model_2).query();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_delete, commit_NoThrow)
{
	ASSERT_NO_THROW(this->query->use(this->driver).commit());
}
