#include "nope.h"
#include "nopeapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <openssl/sha.h>

static void form(Request *request, Response *response);
static void shorten (char *longUrl, char *shortUrl);
static void getLongUrl (char *shortUrl, char *longUrl);
static int callback(void *data, int argc, char **argv, char **azColName);
static char* hash_generator(char *word);

//global variables
int query_stat = 0;
char *result = NULL;
char hash_in[255], URL_in[255];
char *hash, *URL;
sqlite3 *db;
char *zErrMsg = 0;
int rc;
char *sql;
const char* data;
char *longUrl, *shortUrl;
char *baseUrl = "www.tinyURL.com/";

void server(Request * request, Response * response)
{
	printf("%s %s\n",request->method,request->reqStr);

    if (routeRequest(request, response,"/shorten", form)||routeRequest(request, response,"/", form)) {
        //Request to shorten the URL
    } else {
        response->apiFlags |= API_FLAGS_DONT_SET_HEADER_BEFORE_SENDING;
        char * queryPath = getQueryPath(request);
        char longUrl[4096];
        getLongUrl(queryPath,longUrl);
        STATIC_SEND(response->fd, "HTTP/1.1 301 Moved Permanently\r\n");
        resPrintf(response, "Location: %s\r\n", longUrl);
        STATIC_SEND(response->fd, "\r\n");
        free(queryPath);
    }


}

static void form(Request *request, Response *response)
{
    char *nStr = QUICK_FORM_TEXT(request,response,"longurl");
    char shortUrl[6];

    if (nStr != NULL) {
        shorten(nStr,shortUrl);
        resPrintf(response, "Here's the short url of %s: ", nStr);
        resPrintf(response,STAG(input,type="text" disabled="disabled" value="%s"),shortUrl);

    }
}

static void shorten (char *longUrl, char *shortUrl) {
    //URL Shortening logic goes here
	char sql_chkURL[255] = "SELECT Hash from Records where URL = '";
		strcat(sql_chkURL, longUrl);
		strcat(sql_chkURL, "';");
		//printf("%s\n", sql_chkURL);


		char *con_url[255];
		strcat(con_url,baseUrl);
		hash = hash_generator(longUrl);

		rc = sqlite3_exec(db, sql_chkURL, callback, 0, &zErrMsg);
		//printf("%d", query_stat);
		if (query_stat != 0) { //Url is on database
			shortUrl = result;
			query_stat = 0;
			result = NULL;

			strcat(con_url,shortUrl);
			//printf("the tiny url for that :%s\n",shortUrl);

		} else { //url not in database
			//collition resolving and hash generating
			while (1) {
				sql_chkURL[255] = "SELECT Hash from Records where Hash = '";
				strcat(sql_chkURL, hash);
				strcat(sql_chkURL, "';");
				rc = sqlite3_exec(db, sql_chkURL, callback, 0, &zErrMsg);
				if (query_stat == 0)
					break;
				hash = hash_generator(hash);
			}
			query_stat = 0;
			result = NULL;
			strcat(con_url,hash);

			char sql_in_1[255] = "INSERT OR IGNORE INTO Records(Hash,URL) VALUES ('";
			strcat(sql_in_1, hash);
			strcat(sql_in_1, "'");
			strcat(sql_in_1, ",'");
			strcat(sql_in_1, longUrl);
			strcat(sql_in_1, "');");

			//printf("done %s\n ", sql_in_1);

			rc = sqlite3_exec(db, sql_in_1, callback, 0, &zErrMsg);
			//Execute the sql statement.
			if (rc != SQLITE_OK) {
				fprintf(stderr, "SQL error2: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
			} else {
				fprintf(stdout, "database updated\n");
			}
		}
		//assign result to shortUrl
		shortUrl = con_url;
}

static void getLongUrl (char *shortUrl, char *longUrl) {
    //Long URL fetching logic goes here
	// this shortUrl = saved hash of longurl and it may not included base Url
	char sql_getLong[255] = "SELECT URL from Records where Hash = '";
	strcat(sql_getLong, shortUrl);
	strcat(sql_getLong, "';");

	rc = sqlite3_exec(db, sql_getLong, callback, 0, &zErrMsg);
	//Execute the sql statement.
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error2: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	longUrl = result;
	result = NULL;
	query_stat = 0;
}

static int callback(void *data, int argc, char **argv, char **azColName) {
// this function can use for print the database retrivals.
	query_stat = argc;
	result = argv[0];
	return 0;
}

static char* hash_generator(char *word) {
//this function for create the hash value for string
	int i = 0;
	unsigned char temp[SHA_DIGEST_LENGTH];
	char buf[SHA_DIGEST_LENGTH * 2];
	memset(buf, 0x0, SHA_DIGEST_LENGTH * 2);
	memset(temp, 0x0, SHA_DIGEST_LENGTH);

//hash generate using SHA1 algorithm
	SHA1((unsigned char *) word, strlen(word), temp);
	for (i = 0; i < 4; i++) {
		sprintf((char*) &(buf[i]), "%02x", temp[i]);
	}

	char* out_hash = malloc(strlen(buf) + 1);
	strcpy(out_hash, buf);

	return out_hash;
}
