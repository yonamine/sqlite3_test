/**
 * 
 * 
 * 
 * sqlite > .mode column
 * sqlite > .headers on
 * sqlite > SELECT * FROM Cars;
 * 
 * 
 * char *sql = "CREATE TABLE Friends(Id INTEGER PRIMARY KEY, Name TEXT);"
 *             "INSERT INTO Friends(Name) VALUES ('Tom');"
 *             "INSERT INTO Friends(Name) VALUES ('Rebecca');"
 *             "INSERT INTO Friends(Name) VALUES ('Jim');"
 *             "INSERT INTO Friends(Name) VALUES ('Roger');"
 *             "INSERT INTO Friends(Name) VALUES ('Robert');";
 * 
 */
#include <iostream>
#include <string>

#include <memory>

#include <algorithm>
#include <functional>

#include <vector>
#include <list>

#include <sstream>
#include <fstream>

#include <cstdlib>

#include "sqlite3.h"


#define STR_NAME_SIZE	128
struct City {
	char name[STR_NAME_SIZE];
	int population;
};



/**
 *
 *
 */
class CityDAO {
public:
	/**
	 *
	 */
	CityDAO() = default;
	~CityDAO() = default;

	/**
	 *
	 */
	bool LoadCSVFile();
	/**
	 *
	 */
	void UnloadCSVFile();
	/**
	 *
	 */
	std::vector<std::shared_ptr<City>> getCities() { return cities; }

private:
	std::vector<std::shared_ptr<City>> cities;
};


bool CityDAO::LoadCSVFile() {
	std::string line;
	std::ifstream is;

	is.open("city.csv");
	if(!is.is_open()) {
		printf("Invalid filename!\n");
		return false;
	}

	// Skip the 1st line (headers)
	std::getline(is, line);

	while(std::getline(is, line)) {
		std::stringstream ss(line);
		std::string token;

		std::shared_ptr<City> city = std::make_shared<City>();

		if(std::getline(ss, token, ',')) {
			strncpy(city->name, token.c_str(), STR_NAME_SIZE);
		}

		if(std::getline(ss, token, ',')) {
			city->population = stoi(token);
		}

		cities.push_back(city);
	}

	is.close();
	return true;
}


void CityDAO::UnloadCSVFile() {
	while(!cities.empty()) {
		cities.pop_back();
	}
}



/**
 *
 *
 * References:
 * [1] https://www.sqlite.org/copyright.html
 * [2] https://www.sqlite.org/lang_createtable.html
 * [3] https://www.guru99.com/sqlite-query.html
 *
 *
 */


/**
 *
 */
class CityDB {
public:
	static const std::string db_filename;
	static const std::string table_name;

	/**
	 *
	 */
	CityDB() : db(nullptr), is_opened(false) { }
	/**
	 *
	 */
	~CityDB();
	/**
	 *
	 */
	bool open();
	/**
	 *
	 */
	bool close();
	/**
	 *
	 */
	void Insert(std::vector<std::shared_ptr<City>> cities);
	/**
	 *
	 */
	void Query();
private:
	sqlite3 *db;
	bool is_opened;

	/**
	 *
	 */
	static int callback(void *NotUsed, int argc, char **argv, char **azColName);
	/**
	 *
	 */
	void createTable();
	/**
	 *
	 */
	void dropTable();
};

const std::string CityDB::db_filename{ "cities.db" };
const std::string CityDB::table_name{ "cities" };

CityDB::~CityDB() {
	close();
}

int CityDB::callback(void *NotUsed, int argc, char **argv, char **azColName) {
	for(int i = 0; i < argc; i++) {
		printf("%s: %s\n", azColName[i], (argv[i] ? argv[i] : "NULL"));
	}
	return 0;
}

bool CityDB::open() {
	char *zErrMsg = 0;
	int rc = -1;

	// // Open SQLite database is represented by a database handle
	// sqlite3_stmt *res;
	// // Open a new database connection: database namd and database handle
	// rc = sqlite3_open(":memory:", &db); // <- in-memory database
	// // get the version of SQLite Library
	// rc = sqlite3_prepare_v2(db, "SELECT SQLITE_VERSION()", -1, &res, 0);
	// // db - database handler
	// // SELECT SQLITE_VERSION() - SQL statement to be compiled
	// //  -1 - maximum length of the SQL statement in bytes. -1 means to read until '\0'.
	// //       According to the specification, specifying the length may get some performance advantage
	// // res - statement handle. It points to the precompiled statement if sqlite3_prepare_v2 
	// //       runs successfully
	// //   0 - pointer to unused portion of the SQL statement
    // if (rc != SQLITE_OK) {
    //     fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
    //     sqlite3_close(db);
    //     return 1;
    // }
	// // Run the SQL statement
    // rc = sqlite3_step(res);
    // if (rc == SQLITE_ROW) {
    //     printf("%s\n", sqlite3_column_text(res, 0));
    // }
    

	rc = sqlite3_open(db_filename.c_str(), &db);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return false;
	}

	createTable();

	is_opened = true;
	return true;
}

bool CityDB::close() {
	if(is_opened) {
		dropTable();
		sqlite3_close(db);
		is_opened = false;
		return true;
	}
	return false;
}

void CityDB::createTable() {
	char *zErrMsg = 0;
	// CREATE TABLE IF NOT EXISTS fruits_tbl(id integer not null primary key desc, fruta varchar(128), publish_timestamp timestamp default current_timestamp, publish_date date, publish_time time)
	// CREATE TABLE IF NOT EXISTS fruits_tbl(fruta varchar(128), publish_timestamp timestamp default current_timestamp, publish_date date, publish_time time)
	std::string stmt = "CREATE TABLE IF NOT EXISTS " + table_name + "(name TEXT NOT NULL, population INTEGER NOT NULL)";
	printf("%s\n", stmt.c_str());
	// 'sqlite3_exec' is a wrapper to 'sqlite3_prepare_v2()', 'sqlite3_ste()' and 'sqlite3_finalize'
	// it can run multiple SQL multiple statements in C++ string variable
	// callback function invoked for each result row comming out of the evaluated SQL statement
	// 0 is the 1st parameter to the callback function
	int rc = sqlite3_exec(db, stmt.c_str(), &callback, 0, &zErrMsg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}

void CityDB::dropTable() {
	char *zErrMsg = 0;
	std::string stmt = "DROP TABLE IF EXISTS " + table_name;
	printf("%s\n", stmt.c_str());
	int rc = sqlite3_exec(db, stmt.c_str(), &callback, 0, &zErrMsg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}

void CityDB::Query() {
	char *zErrMsg = 0;
	// std::string stmt = "SELECT * FROM " + table_name + " WHERE name = \'Yonkers\'";
	std::string stmt = "SELECT * FROM " + table_name + " WHERE name like \'S%\' and population > 400000";
	printf("%s\n", stmt.c_str());
	int rc = sqlite3_exec(db, stmt.c_str(), &callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}

void CityDB::Insert(std::vector<std::shared_ptr<City>> cities) {
	for_each(cities.begin(), cities.end(), [&](std::shared_ptr<City>city) {
		char *zErrMsg = 0;
		std::string stmt = "insert into " + table_name + "(name, population) values ('" + std::string(city->name) + "', " + std::to_string(city->population) + ")";
		printf("%s\n", stmt.c_str());
		int rc = sqlite3_exec(db, stmt.c_str(), callback, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		// Returns the row ID of the most recent successfull insert into the table
		// int last_id = sqlite3_last_insert_rowid(db);
		// printf("The last Id of the inserted row is %d\n", last_id);
	});
}






/**
 *
 *
 *
 *
 */
int main(int argc, char **argv) {
	CityDAO city_dao;

	printf("sqlite3_libversion: '%s'\n", sqlite3_libversion());

	if(city_dao.LoadCSVFile()) {
		std::vector<std::shared_ptr<City>> cities = city_dao.getCities();

		city_dao.UnloadCSVFile();

		CityDB db;
		db.open();
		db.Insert(cities);
		db.Query();
		db.close();
	}

	return EXIT_SUCCESS;
}