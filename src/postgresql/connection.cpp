/**
 * postgresql/connection.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./connection.h"

#ifdef USE_POSTGRESQL


__ORM_POSTGRESQL_BEGIN__

PostgreSQLConnection::PostgreSQLConnection(const PostgreSQLCredentials& credentials) :
	in_transaction(false)
{
	credentials.validate();
	this->db.reset(
		PQsetdbLogin(
			credentials.host.c_str(),
			std::to_string(credentials.port).c_str(),
			nullptr, nullptr,
			credentials.name.c_str(),
			credentials.user.c_str(),
			credentials.password.c_str()
		),
		[&](PGconn* conn)
		{
			try
			{
				if (this->in_transaction)
				{
					this->rollback_transaction();
				}
			}
			catch (std::exception& exc)
			{
				PQfinish(conn);
				throw;
			}
		}
	);

	if (PQstatus(this->db.get()) != CONNECTION_OK && PQsetnonblocking(this->db.get(), 1) != 0 )
	{
		throw DatabaseError(PQerrorMessage(this->db.get()));
	}
}

void PostgreSQLConnection::run_query(
	const std::string& sql_query,
	const std::function<void(const std::map<std::string, char*>& /* columns */)>& row_handler,
	const std::function<void(const std::vector<char*>& /* columns */)>& vector_handler
) const
{
	try
	{
		this->run_query_unsafe(sql_query, row_handler, vector_handler);
	}
	catch (const std::exception& exc)
	{
		this->rollback_transaction();
		throw;
	}
}

void PostgreSQLConnection::run_query(const std::string& sql_query, std::string& last_row_id) const
{
	this->run_query(sql_query, nullptr, nullptr);
}

void PostgreSQLConnection::run_query_unsafe(
	const std::string& query,
	std::function<void(const std::map<std::string, char*>&)> map_handler,
	std::function<void(const std::vector<char*>&)> vector_handler
) const
{
	if (query.empty())
	{
		this->throw_empty_arg("query", _ERROR_DETAILS_);
	}

	auto res = PQexec(this->db.get(), query.c_str());
	auto result_status = PQresultStatus(res);
	if (result_status == PGRES_TUPLES_OK)
	{
		if (map_handler || vector_handler)
		{
			auto fields_count = PQnfields(res);
			auto tuples_count = PQntuples(res);
			for (auto i = 0; i < tuples_count; i++)
			{
				if (map_handler)
				{
					std::map<std::string, char*> result;
					for (auto j = 0; j < fields_count; j++)
					{
						result[PQfname(res, j)] = PQgetvalue(res, i, j);
					}

					map_handler(result);
				}
				else if (vector_handler)
				{
					std::vector<char*> result;
					result.reserve(fields_count);
					for (auto j = 0; j < fields_count; j++)
					{
						result.push_back(PQgetvalue(res, i, j));
					}

					vector_handler(result);
				}
			}
		}
	}
	else if (result_status == PGRES_FATAL_ERROR)
	{
		throw SQLError(PQresultErrorMessage(res), _ERROR_DETAILS_);
	}

	PQclear(res);
}

__ORM_POSTGRESQL_END__

#endif // USE_POSTGRESQL
