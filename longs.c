#include "wafer.h"
#include "waferapi.h"
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <string.h>
#include "styles.h"
#include "longs.h"


void server(Request * request, Response * response) {
	// open sqlite3 database "URL_Records"

	char *envBaseUrl = getenv("BASE_URL");
	if (envBaseUrl != NULL) {
        baseUrl = envBaseUrl;
	}

	char *envDbPath = getenv("DB_PATH");
	if (envDbPath != NULL) {
        dbPath = envDbPath;
	}

	char *envHashLength = getenv("HASH_LENGTH");
	if (envHashLength != NULL) {
        hashLength = strtol(envHashLength, (char **)NULL, 10);
	}

	int rc = sqlite3_open(dbPath, &db);
	// if db exist it will open otherwise create a db
	if (rc) {
		fprintf(stderr, "sqlite db connectivity error.");
		exit(0);
	}
	//query to create "Records" table if not exists.
	char * sql = "CREATE TABLE if not exists Records(URL TEXT PRIMARY KEY NOT NULL);";

    char * zErrMsg;
    Params params;
	rc = sqlite3_exec(db, sql, NULL, NULL, &zErrMsg);
	//Execute the sql statement.
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL table creating error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	dbgprintf("%s %s\n", request->method, request->reqStr);

	if (routeRequest(request, response, "/shorten", form)
			|| routeRequest(request, response, "/", form)) {
		//Request to shorten the URL
	} else if (routeRequest(request, response, "/favicon.ico", favicon)) {

	} else {

		char * queryPath = getQueryPath(request);
		params.response = response;
		getLongUrl(queryPath, &params);
        free(queryPath);
	}

	sqlite3_close(db);
}

static void favicon(Request *request, Response *response) {

}

static void form(Request *request, Response *response) {
    resPrintf(response,HEADER_LAYOUT,"Longs");
	char *longUrl = QUICK_FORM_TEXT(request, response, "URL to shorten ");
	dbgprintf("form  longUrl %s\n",longUrl);
	if (longUrl != NULL) {
		shorten(longUrl, response);
	}
	free(longUrl);
	resPrintf(response,WELCOME_PAGE);
	resPrintf(response,FOOTER_LAYOUT,2016);
}

static void shorten(char *longUrl, Response *response) {

    //Validate url
    if (!validate_url(longUrl)) {
        resPrintf(response, "Must be a valid URL.");
        return;
    }

    sqlite3_stmt *ppStmt;

    //To prevent bobbytabling, we encode the URL
    int rc;

    //Try to insert it. Dupes will be ignored.
    sqlite3_prepare_v2(db, "INSERT OR IGNORE INTO Records(URL) VALUES (?1);", -1, &ppStmt, NULL);
    sqlite3_bind_text(ppStmt, 1, longUrl, -1, SQLITE_STATIC);

    rc = sqlite3_step(ppStmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL query execution error: INSERT OR IGNORE INTO Records(URL) VALUES (?1); : %s\n",  sqlite3_errmsg(db));
        goto out;
    }

    sqlite3_finalize(ppStmt);

    //Fetch it again
    sqlite3_prepare_v2(db, "SELECT ROWID from Records where URL = ?1;", -1, &ppStmt, NULL);
    sqlite3_bind_text(ppStmt, 1, longUrl, -1, SQLITE_STATIC);


    rc = sqlite3_step(ppStmt);

    if (rc == SQLITE_ROW) {
        int id = sqlite3_column_int(ppStmt, 0);
        char hash[hashLength+1];
		alphabet_base_encode(hash, id);
		dbgprintf("%s\n",hash);
		Params params;
		params.longUrl = longUrl;
		params.response = response;
        printShortUrl(hash, &params) ;
    } else if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL query execution error: SELECT ROWID from Records where URL = ?1; : %s\n",  sqlite3_errmsg(db));
        goto out;
    }

out:
    sqlite3_close(db);
    sqlite3_finalize(ppStmt);
}

static void getLongUrl(char *shortUrl, Params * params) {
    sqlite3_stmt *ppStmt;
    long id = alphabet_base_decode(shortUrl);
    sqlite3_prepare_v2(db, "SELECT URL from Records where ROWID = ?1;", -1, &ppStmt, NULL);
    sqlite3_bind_int(ppStmt, 1, id);
    int rc = sqlite3_step(ppStmt);

    if (rc == SQLITE_ROW) {
        params->response->apiFlags |= API_FLAGS_DONT_SET_HEADER_BEFORE_SENDING;
        STATIC_SEND(params->response->fd, "HTTP/1.1 301 Moved Permanently\r\n");
        STATIC_SEND(params->response->fd, "Content-Type: */*; charset=\"UTF-8\"\r\n");
        STATIC_SEND(params->response->fd, "Content-Length: 0\r\n");
		resPrintf(params->response, "Location: %s\r\n", sqlite3_column_text(ppStmt, 0));
		STATIC_SEND(params->response->fd, "\r\n");
    } else {
        resPrintf(params->response, HEADER_LAYOUT,"Longs");
        resPrintf(params->response, "We have no record of that short URL<em>%s</em> ", shortUrl);
        resPrintf(params->response, WELCOME_PAGE);
        resPrintf(params->response,FOOTER_LAYOUT,2016);
    }
    sqlite3_finalize(ppStmt);
    sqlite3_close(db);
}

static void printShortUrl(char * hash, Params * params) {
            char shortUrl[SIZE];
            snprintf(shortUrl, SIZE, "%s/%s",baseUrl,hash);
            resPrintf(params->response, "Here's the short url of <em>%s</em> ", params->longUrl);
            resPrintf(params->response,
				STAG(input, type="text" readonly="readonly" value="%s"),
				shortUrl);
}

/* Encoding/Decoding Tables */

static char* alphabet_base_encode(char *hash1, int id) {
	int i;
	dbgprintf("%d\n", id);

	for (i=0;i<hashLength;i++) {
        hash1[i]=alphebet[id/powerof32[(MAX_HASH_LENGTH-hashLength)+i]];
        id %= powerof32[(MAX_HASH_LENGTH-hashLength)+i];
	}
	hash1[i]='\0';
	dbgprintf("%s\n", hash1);

	return hash1;
}

long alphabet_base_decode(const char *hash1) {
	int i;
	long id = 0;
	for (i=0;i<hashLength;i++) {
        id += powerof32[(MAX_HASH_LENGTH-hashLength)+i]*alphabet_table[hash1[i+1]];
	}
	dbgprintf("%d\n", id);
	return id;
}

bool validate_url(char *c) {
    char * http_string = "http";
    int c_len = strlen(c);
    int i;

    for (i=0;i<4;i++) {
        if (i>=c_len||c[i]!=http_string[i]) {
            dbgprintf ("Invalid Scheme http A %c, E %c, %d\n",c[i],http_string[i],i);
            return false;
        }
    }
    if (i<c_len&&c[i]=='s') {
        i++;
    }
    int cursor = i;
    char * colslashslash = "://";
    for (i=cursor;i<cursor+3;i++) {
        if (i>=c_len||c[i]!=colslashslash[i-cursor]) {
            dbgprintf ("Invalid Scheme :// A %c, E %c, %d\n",c[i],colslashslash[i-cursor],i);
            return false;
        }
    }

    cursor=i;

    for (i=cursor;i<c_len;i++) {
        if (!valid_html_chars[c[i]]) {
            dbgprintf ("Invalid Scheme %c, %d\n",c[i],i);
            return false;
        }
    }
    return true;
}
