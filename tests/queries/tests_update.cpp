/**
 * queries/tests_delete.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/update.h"

#include "./mocked_backend.h"

using namespace xw;


struct TestCase_Q_update_TestModel : public orm::db::Model
{
	int id{};
	std::string name;

	static constexpr const char* meta_table_name = "test";

	inline static const std::tuple meta_columns = {
		orm::db::make_pk_column_meta("id", &TestCase_Q_update_TestModel::id),
		orm::db::make_column_meta("name", &TestCase_Q_update_TestModel::name)
	};

	inline void __orm_set_column__(const std::string& column_name, const char* data) override
	{
		this->__orm_set_column_data__(TestCase_Q_update_TestModel::meta_columns, column_name, data);
	}
};

class TestCaseF_Q_update : public ::testing::Test
{
protected:
	MockedBackend* backend;
	std::shared_ptr<orm::abc::IDatabaseConnection> conn;

	void SetUp() override
	{
		this->backend = new MockedBackend();
		this->conn = this->backend->get_connection();
	}

	void TearDown() override
	{
		this->backend->release_connection(this->conn);
		delete this->backend;
	}
};

TEST_F(TestCaseF_Q_update, constructor_ThrowsNullModel)
{
	TestCase_Q_update_TestModel model;
	model.mark_as_null();
	ASSERT_THROW(auto _ = orm::q::Update<TestCase_Q_update_TestModel>(
		this->conn.get(), this->backend->sql_builder()
	).model(model), orm::QueryError);
}

TEST_F(TestCaseF_Q_update, commit_one_ThrowsMultipleModelsWereSet)
{
	TestCase_Q_update_TestModel model_1, model_2;
	ASSERT_THROW(orm::q::Update<TestCase_Q_update_TestModel>(
		this->conn.get(), this->backend->sql_builder()
	).model(model_1).model(model_2).commit_one(), orm::QueryError);
}

TEST_F(TestCaseF_Q_update, query_SingleRow)
{
	TestCase_Q_update_TestModel model;
	model.id = 1;
	model.name = "John";

	auto expected = R"(UPDATE "test" SET name = 'John' WHERE "test"."id" = 1;)";
	auto actual = orm::q::Update<TestCase_Q_update_TestModel>(
		this->conn.get(), this->backend->sql_builder()
	).model(model).to_sql();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_update, query_MultipleRows)
{
	TestCase_Q_update_TestModel model_1;
	model_1.id = 1;
	model_1.name = "John";

	TestCase_Q_update_TestModel model_2;
	model_2.id = 2;
	model_2.name = "Steve";

	auto expected = R"(UPDATE "test" SET name = 'John' WHERE "test"."id" = 1; UPDATE "test" SET name = 'Steve' WHERE "test"."id" = 2;)";
	auto actual = orm::q::Update<TestCase_Q_update_TestModel>(
		this->conn.get(), this->backend->sql_builder()
	).model(model_1).model(model_2).to_sql();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_update, commit_one_NoThrow)
{
	TestCase_Q_update_TestModel model;
	model.id = 1;
	model.name = "John";

	ASSERT_NO_THROW(orm::q::Update<TestCase_Q_update_TestModel>(
		this->conn.get(), this->backend->sql_builder()
	).model(model).commit_one());
}

TEST_F(TestCaseF_Q_update, commit_batch_NoThrow)
{
	TestCase_Q_update_TestModel model_1;
	model_1.id = 1;
	model_1.name = "John";

	TestCase_Q_update_TestModel model_2;
	model_2.id = 2;
	model_2.name = "Steve";

	ASSERT_NO_THROW(orm::q::Update<TestCase_Q_update_TestModel>(
		this->conn.get(), this->backend->sql_builder()
	).model(model_1).model(model_2).commit_batch());
}
