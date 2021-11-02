/**
 * queries/tests_insert.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/insert.h"

#include "./mocked_backend.h"

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

	inline void __orm_set_column__(const std::string& column_name, const char* data) override
	{
		this->__orm_set_column_data__(TestCase_Q_insert_TestModel::meta_columns, column_name, data);
	}
};

class TestCase_Q_insert_One : public ::testing::Test
{
protected:
	MockedBackend* backend;
	orm::q::Insert<TestCase_Q_insert_TestModel>* query;
	std::shared_ptr<orm::IDatabaseConnection> conn;

	void SetUp() override
	{
		this->backend = new MockedBackend();
		this->conn = this->backend->get_connection();
		this->query = new orm::q::Insert<TestCase_Q_insert_TestModel>(
			this->conn.get(), this->backend->sql_builder()
		);
		this->query->model(TestCase_Q_insert_TestModel());
	}

	void TearDown() override
	{
		this->backend->release_connection(this->conn);
		delete this->query;
		delete this->backend;
	}
};

class TestCase_Q_insert_Bulk : public ::testing::Test
{
protected:
	MockedBackend* backend;
	orm::q::Insert<TestCase_Q_insert_TestModel>* query;
	std::shared_ptr<orm::IDatabaseConnection> conn;

	void SetUp() override
	{
		this->backend = new MockedBackend();
		this->conn = this->backend->get_connection();
		this->query = new orm::q::Insert<TestCase_Q_insert_TestModel>(
			this->conn.get(), this->backend->sql_builder()
		);
		this->query->model(TestCase_Q_insert_TestModel());
		this->query->model(TestCase_Q_insert_TestModel());
	}

	void TearDown() override
	{
		delete this->query;
		this->backend->release_connection(this->conn);
		delete this->backend;
	}
};

TEST_F(TestCase_Q_insert_Bulk, commit_one_FailDueToBulkMode)
{
	ASSERT_THROW(this->query->commit_one(), orm::QueryError);
}

class TestCaseF_Q_insert : public ::testing::Test
{
protected:
	MockedBackend* backend;

	void SetUp() override
	{
		this->backend = new MockedBackend();
	}

	void TearDown() override
	{
		delete this->backend;
	}
};

TEST_F(TestCaseF_Q_insert, query_SingleModel)
{
	TestCase_Q_insert_TestModel model;
	model.name = "Steve";

	auto expected = R"(INSERT INTO "test_models" (name) VALUES ('Steve');)";
	auto w = this->backend->wrap_connection();
	auto actual = orm::q::Insert<TestCase_Q_insert_TestModel>(
		w.connection(), this->backend->sql_builder()
	).model(model).to_sql();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_insert, query_MultipleModels)
{
	TestCase_Q_insert_TestModel model_1;
	model_1.name = "Steve";

	TestCase_Q_insert_TestModel model_2;
	model_2.name = "John";

	auto expected = R"(INSERT INTO "test_models" (name) VALUES ('Steve'), ('John');)";
	auto w = this->backend->wrap_connection();
	auto actual = orm::q::Insert<TestCase_Q_insert_TestModel>(
		w.connection(), this->backend->sql_builder()
	).model(model_1).model(model_2).to_sql();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_insert, commit_one_ReturnedStringPk)
{
	TestCase_Q_insert_TestModel model;
	model.name = "Steve";

	std::string pk;
	auto w = this->backend->wrap_connection();
	orm::q::Insert<TestCase_Q_insert_TestModel>(
		w.connection(), this->backend->sql_builder()
	).model(model).commit_one(pk);
	ASSERT_EQ(pk, "1");
}

TEST_F(TestCaseF_Q_insert, commit_one_SetPk)
{
	TestCase_Q_insert_TestModel model;
	model.id = 0;
	model.name = "Steve";

	auto w = this->backend->wrap_connection();
	ASSERT_NO_THROW(orm::q::Insert<TestCase_Q_insert_TestModel>(
		w.connection(), this->backend->sql_builder()
	).model(model).commit_one(model.id));
	ASSERT_EQ(model.id, 1);
}

TEST_F(TestCaseF_Q_insert, commit_batch_NoThrow)
{
	TestCase_Q_insert_TestModel model_1;
	model_1.name = "Steve";

	TestCase_Q_insert_TestModel model_2;
	model_2.name = "John";

	auto w = this->backend->wrap_connection();
	ASSERT_NO_THROW(orm::q::Insert<TestCase_Q_insert_TestModel>(
		w.connection(), this->backend->sql_builder()
	).model(model_1).model(model_2).commit_batch());
}
