#include "nope.h"
#include "nopeapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

static void form(Request *request, Response *response);
static void shorten(char *longUrl, char *shortUrl);
static void getLongUrl(char *shortUrl, char *longUrl);
static int callback(void *data, int argc, char **argv, char **azColName);
static char* hash_new_function(int id);

int query_stat = 0;
char *result = NULL; // global variables for db retrivals

sqlite3 *db;  // db initialisation
char *zErrMsg = 0; // db error msg initialisation
int rc;
char *sql;
const char* data;
char *longUrl, *shortUrl;
char *baseUrl = "www.tinyURL.com/"; // base url of shorted urls

void server(Request * request, Response * response) {
	// open sqlite3 database "URL_Records"
	rc = sqlite3_open("URL_Records.db", &db);
	// if db exist it will open otherwise create a db
	if (rc) {
		fprintf(stderr, "sqlite db connectivity error.");
		exit(0);
	}
	//query to create "Records" table if not exists.
	sql = "CREATE TABLE if not exists Records( "
			"ID INTEGER PRIMARY KEY  AUTOINCREMENT,"
			"Hash CHAR(5)  NOT NULL,URL TEXT NOT NULL);";

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	//Execute the sql statement.
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL table creating error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	printf("%s %s\n", request->method, request->reqStr);

	if (routeRequest(request, response, "/shorten", form)
			|| routeRequest(request, response, "/", form)) {
		//Request to shorten the URL
	} else {
		response->apiFlags |= API_FLAGS_DONT_SET_HEADER_BEFORE_SENDING;
		char * queryPath = getQueryPath(request);
		char longUrl[4096];
		getLongUrl(queryPath, longUrl);
		STATIC_SEND(response->fd, "HTTP/1.1 301 Moved Permanently\r\n");
		resPrintf(response, "Location: %s\r\n", longUrl);
		STATIC_SEND(response->fd, "\r\n");
		free(queryPath);
	}
	sqlite3_close(db);

}

static void form(Request *request, Response *response) {
	char *nStr = QUICK_FORM_TEXT(request, response, "longurl");
	char shortUrl[6];

	if (nStr != NULL) {
		shorten(nStr, shortUrl);
		resPrintf(response, "Here's the short url of %s: ", nStr);
		resPrintf(response,
				STAG(input, type="text" disabled="disabled" value="%s"),
				shortUrl);

	}
}

static void shorten(char *longUrl, char *shortUrl) {
	char *hashnew, *shortUrl1;

	//create query for check longurl exists or not and if exists retrive the hash of url
	char sql_chkURL[255] = "SELECT Hash from Records where URL = '";
	strcat(sql_chkURL, longUrl);
	strcat(sql_chkURL, "';");

	rc = sqlite3_exec(db, sql_chkURL, callback, 0, &zErrMsg);
	//Execute the sql statement.
	if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL query execution error id = 01 : %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}

	if (query_stat != 0) { //Url exists in database
		shortUrl1 = result; //assign hash to shorturl back part
		query_stat = 0;
		result = NULL; // get free global variables

	} else { //url doesn't exists in database
		//query for get next db entry ID
		char sql_chkURL11[255] = "SELECT COALESCE(MAX(ID)+1, 0) FROM  Records";

		rc = sqlite3_exec(db, sql_chkURL11, callback, 0, &zErrMsg);
		//Execute the sql statement.
		if (rc != SQLITE_OK) {
					fprintf(stderr, "SQL query execution error id = 02 : %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
		}

		int id_entry = atoi(result);//db retrived string convert into integer
		if (id_entry == 0)id_entry = 1; // remove the id entry zero

		hashnew = hash_new_function(id_entry);//generate the hash for url according to the ID

		query_stat = 0;
		result = NULL;// get free global variables
		shortUrl1 = hashnew; //new hash assign to back part of short url

		//query for insert new url entry to the database.
		char sql_in_1[255] = "INSERT OR IGNORE INTO Records(Hash,URL) VALUES ('";
		strcat(sql_in_1, hashnew);
		strcat(sql_in_1, "'");
		strcat(sql_in_1, ",'");
		strcat(sql_in_1, longUrl);
		strcat(sql_in_1, "');");

		rc = sqlite3_exec(db, sql_in_1, callback, 0, &zErrMsg);
		//Execute the sql statement.
		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL query execution error id = 03 : %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		} else {
			fprintf(stdout, "database updated\n");
			shortUrl1 = hashnew;
		}
	}
	//assign result to shortUrl
	char str_output[20] = { 0 };
	strcpy(str_output, baseUrl);
	strcat(str_output, shortUrl1);
	shortUrl = str_output;
	//printf("The tiny url  :%s\n", shortUrl);
}

static void getLongUrl(char *shortUrl, char *longUrl) {
	// query for retrive the url
	char sql_getLong[255] = "SELECT URL from Records where Hash = '";
	strcat(sql_getLong, shortUrl);
	strcat(sql_getLong, "';");

	rc = sqlite3_exec(db, sql_getLong, callback, 0, &zErrMsg);
	//Execute the sql statement.
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL query execution error id = 04 : %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	longUrl = result; //asign db retival into longurl
	//printf("long url %s,\n", longUrl);
	result = NULL;
	query_stat = 0;//get free global variables
}

static int callback(void *data, int argc, char **argv, char **azColName) {

	query_stat = argc;
	result = argv[0];
	/*This function can use for print the database retrivals.
	 * if you want to print all the retrivals from query
	 * int i;
	 for (i = 0; i < argc; i++) {
	 printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	 }*/
	return 0;
}

static char* hash_new_function(int id) {
	int temp_id = id;
	char hash1[5];
	int p = 0, c = 0;
	// 62base alphebet set as char array
	char alphebet[62] =
			"abcdefghijklmnopkrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

	//hash generating logic
	while (id > 0) {
		hash1[p] = alphebet[id % 62];
		p++;
		id = id / 62;
	}
	// generated hash get reversed and assign to an array
	char hash_out1[5];
	if (temp_id == 0) {
		hash_out1[0] = alphebet[0];
	} else {
		while (p > 0) {
			hash_out1[c++] = hash1[--p];
		}
	}

	char *aaa = hash1;
	char* rtn_hash = malloc(p);//to parse a string throw the functions
	strcpy(rtn_hash, hash_out1);
	return rtn_hash;
}
