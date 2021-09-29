/**
 * queries/tests_delete.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/delete.h"

#include "./mocked_backend.h"

using namespace xw;

struct TestCaseF_Q_delete_TestModel : public orm::db::Model
{
	int id{};
	std::string name;

	static constexpr const char* meta_table_name = "test_models";

	inline static const std::tuple meta_columns = {
		orm::db::make_pk_column_meta("id", &TestCaseF_Q_delete_TestModel::id),
		orm::db::make_column_meta("name", &TestCaseF_Q_delete_TestModel::name)
	};

	inline void __set_attr__(const char* attr_name, const void* data) override
	{
		this->set_attribute_to(TestCaseF_Q_delete_TestModel::meta_columns, attr_name, data);
	}

	[[nodiscard]]
	inline std::shared_ptr<const Object> __get_attr__(const char* attr_name) const override
	{
		return this->get_attribute_from(TestCaseF_Q_delete_TestModel::meta_columns, attr_name);
	}
};

class TestCaseF_Q_delete : public ::testing::Test
{
protected:
	MockedBackend* backend;
	orm::q::Delete<TestCaseF_Q_delete_TestModel>* query;
	std::shared_ptr<orm::abc::IDatabaseConnection> conn;

	void SetUp() override
	{
		TestCaseF_Q_delete_TestModel model;
		model.id = 0;
		model.name = "Steve";
		this->backend = new MockedBackend();
		this->conn = this->backend->get_connection();
		this->query = new orm::q::Delete<TestCaseF_Q_delete_TestModel>(
			conn.get(), this->backend->sql_builder()
		);
		this->query->model(model);
	}

	void TearDown() override
	{
		delete this->query;
		this->backend->release_connection(this->conn);
		delete this->backend;
	}
};

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
	auto actual = this->query->where(orm::q::c(&TestCaseF_Q_delete_TestModel::id) == 1)
		.where(orm::q::like(&TestCaseF_Q_delete_TestModel::name, "%hn")).to_sql();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_delete, constructor_ThrowsNullConnection)
{
	ASSERT_THROW(
		orm::q::Delete<TestCaseF_Q_delete_TestModel>(nullptr, this->backend->sql_builder()),
		NullPointerException
	);
}

TEST_F(TestCaseF_Q_delete, constructor_ThrowsNullQueryBuilder)
{
	ASSERT_THROW(
		orm::q::Delete<TestCaseF_Q_delete_TestModel>(this->conn.get(), nullptr),
		NullPointerException
	);
}

TEST_F(TestCaseF_Q_delete, query_SingleModel)
{
	auto expected = R"(DELETE FROM "test_models" WHERE "test_models"."id" IN (0);)";
	auto actual = this->query->to_sql();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_delete, query_CustomCondition)
{
	auto expected = R"(DELETE FROM "test_models" WHERE "test_models"."name" = 'John';)";
	auto actual = this->query->where(orm::q::c(&TestCaseF_Q_delete_TestModel::name) == "John").to_sql();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_delete, query_MultipleModels)
{
	auto expected = R"(DELETE FROM "test_models" WHERE "test_models"."id" IN (0, 1, 2);)";

	TestCaseF_Q_delete_TestModel model_1;
	model_1.id = 1;

	TestCaseF_Q_delete_TestModel model_2;
	model_2.id = 2;

	auto actual = this->query->model(model_1).model(model_2).to_sql();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_delete, commit_NoThrow)
{
	ASSERT_NO_THROW(this->query->commit());
}
