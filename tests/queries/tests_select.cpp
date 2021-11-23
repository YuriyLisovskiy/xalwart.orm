/**
 * queries/tests_select.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "./mocked_backend.h"
#include "../../src/queries/select.h"

using namespace xw;

struct TestCase_Q_TestModel : public orm::db::Model
{
	static constexpr const char* meta_table_name = "test_model";

	int id{};
	std::string name;

	inline static const std::tuple meta_columns = {
		orm::db::make_pk_column_meta("id", &TestCase_Q_TestModel::id),
		orm::db::make_column_meta("name", &TestCase_Q_TestModel::name)
	};

	inline void __orm_set_column__(const std::string& column_name, const char* data) override
	{
		this->__orm_set_column_data__(TestCase_Q_TestModel::meta_columns, column_name, data);
	}
};

class TestCase_Q_select : public ::testing::Test
{
protected:
	orm::q::Select<TestCase_Q_TestModel>* query;
	std::shared_ptr<orm::IDatabaseConnection> conn;
	MockedBackend* backend;

	void SetUp() override
	{
		this->backend = new MockedBackend();
		this->conn = this->backend->get_connection();
		this->query = new orm::q::Select<TestCase_Q_TestModel>(this->conn.get(), this->backend->sql_builder());
	}

	void TearDown() override
	{
		delete this->query;
		this->backend->release_connection(this->conn);
		delete this->backend;
	}
};

TEST_F(TestCase_Q_select, distinct_NotThrow)
{
	ASSERT_NO_THROW(this->query->distinct());
}

TEST_F(TestCase_Q_select, where_NotThrow)
{
	ASSERT_NO_THROW(this->query->where(orm::q::c(&TestCase_Q_TestModel::id) == 1));
}

TEST_F(TestCase_Q_select, order_by_NotThrow)
{
	ASSERT_NO_THROW(this->query->order_by({orm::q::asc(&TestCase_Q_TestModel::id)}));
}

TEST_F(TestCase_Q_select, order_by_MultipleCalls_NotThrow_WhenFirstCallArgsIsEmpty)
{
	ASSERT_NO_THROW(this->query->order_by({}).order_by({orm::q::asc(&TestCase_Q_TestModel::id)}));
}

TEST_F(TestCase_Q_select, limit_NotThrow)
{
	ASSERT_NO_THROW(this->query->limit(1));
}

TEST_F(TestCase_Q_select, offset_NotThrow)
{
	ASSERT_NO_THROW(this->query->offset(1));
}

TEST_F(TestCase_Q_select, offset_MultipleCalls_NotThrow_WhenPreviousCallsArgsIsEmpty)
{
	ASSERT_NO_THROW(this->query->offset(0).offset(1));
}

TEST_F(TestCase_Q_select, group_by_NotThrow)
{
	ASSERT_NO_THROW(this->query->group_by({"id"}));
}

TEST_F(TestCase_Q_select, group_by_MultipleCalls_NotThrow_WhenFirstCallArgsIsEmpty)
{
	ASSERT_NO_THROW(this->query->group_by({}).group_by({"id"}));
}

TEST_F(TestCase_Q_select, having_NotThrow)
{
	ASSERT_NO_THROW(this->query->having(orm::q::c(&TestCase_Q_TestModel::id) == 1));
}

TEST_F(TestCase_Q_select, distinct_NoThrow_MultipleCalls)
{
	ASSERT_NO_THROW(this->query->distinct().distinct());
}

TEST_F(TestCase_Q_select, where_NoThrow_MultipleCalls)
{
	ASSERT_NO_THROW(this->query->where(orm::q::c(&TestCase_Q_TestModel::id) == 1)
		.where(orm::q::c(&TestCase_Q_TestModel::name) == "John"));
}

TEST_F(TestCase_Q_select, order_by_NoThrow_MultipleCalls)
{
	ASSERT_NO_THROW(
		this->query->order_by({orm::q::asc(&TestCase_Q_TestModel::id)})
			.order_by({orm::q::desc(&TestCase_Q_TestModel::name)})
	);
}

TEST_F(TestCase_Q_select, limit_NoThrow_MultipleCalls)
{
	ASSERT_NO_THROW(this->query->limit(1).limit(2));
}

TEST_F(TestCase_Q_select, offset_NoThrow_MultipleCalls)
{
	ASSERT_NO_THROW(this->query->offset(1).offset(2));
}

TEST_F(TestCase_Q_select, group_by_NoThrow_MultipleCalls)
{
	ASSERT_NO_THROW(this->query->group_by({"id"}).group_by({"name"}));
}

TEST_F(TestCase_Q_select, having_NoThrow_MultipleCalls)
{
	ASSERT_NO_THROW(this->query->having(orm::q::c(&TestCase_Q_TestModel::id) == 1)
		.having(orm::q::c(&TestCase_Q_TestModel::name) == "John"));
}
