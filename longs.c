#include "wafer.h"
#include "waferapi.h"
#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "cencode.h"
#include "cdecode.h"
#include <string.h>
#include "styles.h"

typedef struct {
    Request * request;
    Response * response;
    char * longUrl;
} Params;


static void form(Request *request, Response *response);
static void favicon(Request *request, Response *response) ;
static void shorten(char *longUrl, Response *response);
static void getLongUrl(char *shortUrl, Params * params);
static char * alphabet_base_encode(char *hash1, int id);
static int fetch_id(void *data, int argc, char **argv, char **azColName);
static int fetch_longUrl (void *data, int argc, char **argv, char **azColName);
char* b64encode(const char* input);
char* b64decode(const char* input);
static char* alphabet_base_encode(char *hash1, int id);
long alphabet_base_decode(const char *hash1);


#define HASH_LENGTH 3
#define ALPHABET_BASE
char alphebet[ALPHABET_BASE] = "abcdefghjklmnpkrstuvwxyz23456789";
char alphabet_table [255] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,24,25,26,27,28,29,30,31,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,1,2,
    3,4,5,6,7,-1,8,14,10,11,12,-1,13,-1,15,16,17,18,19,20,21,22,23,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

char valid_html_chars [255] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0
};

long powerof32[HASH_LENGTH] = {1024, 32, 1};


sqlite3 *db;  // db initialisation
char *baseUrl = "lon.gs"; // base url of shorted urls
#define SIZE 2048


void server(Request * request, Response * response) {
	// open sqlite3 database "URL_Records"
	int rc = sqlite3_open("URL_Records.db", &db);
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
		response->apiFlags |= API_FLAGS_DONT_SET_HEADER_BEFORE_SENDING;
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

    char * zErrMsg;

    //To prevent bobbytabling, we encode the URL
    char * b64URL = b64encode(longUrl);
    char * buffer = malloc(SIZE);
    int rc;

    //Try to insert it. Dupes will be ignored.
    snprintf (buffer,SIZE,"INSERT OR IGNORE INTO Records(URL) VALUES ('%s');",b64URL);
    dbgprintf("buffer %s\n",buffer);

	rc = sqlite3_exec(db, buffer, NULL, NULL, &zErrMsg);

	//Execute the sql statement.
	if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL query execution error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
    }

    //Fetch it again
    snprintf (buffer,SIZE,"SELECT ROWID from Records where URL = '%s';",b64URL);
    Params params;
    params.response = response;
    params.longUrl = longUrl;

    rc = sqlite3_exec(db, buffer, fetch_id, &params, &zErrMsg);

    free(buffer);
    free(b64URL);
}

static void getLongUrl(char *shortUrl, Params * params) {
    char * zErrMsg;
    char * buffer = malloc(SIZE);
    long id = alphabet_base_decode(shortUrl);
    snprintf (buffer,SIZE,"SELECT URL from Records where ROWID = %ld;",id);
    int rc = sqlite3_exec(db, buffer, fetch_longUrl, params, &zErrMsg);

	if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL query execution error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
    }

    free(buffer);
}

static void printShortUrl(char * hash, Params * params) {
            char shortUrl[SIZE];
            snprintf(shortUrl, SIZE, "%s/%s",baseUrl,hash);
            resPrintf(params->response, "Here's the short url of <em>%s</em> ", params->longUrl);
            resPrintf(params->response,
				STAG(input, type="text" disabled="disabled" value="%s"),
				shortUrl);
}

static int fetch_longUrl (void *data, int argc, char **argv, char **azColName) {

        int query_stat = argc;
        char * result = argv[0];
        Params * params = (Params *) data;

        char * b64durl = b64decode(result);
        dbgprintf("Retrieved longURL = %s -> %s\n", result, b64durl);
		STATIC_SEND(params->response->fd, "HTTP/1.1 301 Moved Permanently\r\n");
		resPrintf(params->response, "Location: %s\r\n", b64durl);
		STATIC_SEND(params->response->fd, "\r\n");

        free(b64durl);
        return 0;
}

static int fetch_id(void *data, int argc, char **argv, char **azColName) {
        int query_stat = argc;
        char * result = argv[0];
        Params * params = (Params *) data;

        dbgprintf("fetch_id %d %s\n",query_stat,result);
		int id_entry = atoi(result);

        char hash[HASH_LENGTH+1];
		alphabet_base_encode(hash, id_entry);
		dbgprintf("%s\n",hash);
        printShortUrl(hash, params) ;
		return 0;
}


/* Encoding/Decoding Tables */

static char* alphabet_base_encode(char *hash1, int id) {
	int i;
	dbgprintf("%d\n", id);

	for (i=0;i<HASH_LENGTH;i++) {
        hash1[i]=alphebet[id/powerof32[i]];
        id %= powerof32[i];
	}
	hash1[i]='\0';
	dbgprintf("%s\n", hash1);

	return hash1;
}

long alphabet_base_decode(const char *hash1) {
	int i;
	long id = 0;
	for (i=0;i<HASH_LENGTH;i++) {
        id += powerof32[i]*alphabet_table[hash1[i+1]];
	}
	return id;
}

char* b64encode(const char* input)
{
	char* output = (char*)malloc(SIZE);
	char* c = output;
	int cnt = 0;

	base64_encodestate s;

	base64_init_encodestate(&s);

	cnt = base64_encode_block(input, strlen(input), c, &s);
	c += cnt;
	cnt = base64_encode_blockend(c, &s);
	c += cnt;
	*c = 0;

	return output;
}

char* b64decode(const char* input)
{

	char* output = (char*)malloc(SIZE);

	char* c = output;

	int cnt = 0;

	base64_decodestate s;
	base64_init_decodestate(&s);

	cnt = base64_decode_block(input, strlen(input), c, &s);
	c += cnt;
	*c = 0;

	return output;
}

bool validate_url(char *c) {
    char * http_string = "http";
    int c_len = strlen(c);
    int i;

    for (i=0;i<4;i++) {
        if (i>=c_len||c[i]!=http_string[i]) {
            dbgprintf ("Invalid Scheme %cd %c %d\n",c[i],http_string[i],i);
            return false;
        }
    }
    if (i<c_len&&c[i]=='s') {
        i++;
    }
    int cursor = i;
    char * colslashslash = "://";
    for (i=cursor;i<3;i++) {
        if (i>=c_len||c[i]!=colslashslash[i]) {
            dbgprintf ("Invalid Scheme %c %c %d\n",c[i],http_string[i],i);
            return false;
        }
    }

    cursor=i;

    for (i=cursor;i<c_len;i++) {
        if (!valid_html_chars[c[i]]) {
            dbgprintf ("Invalid Scheme %c %d\n",c[i],i);
            return false;
        }
    }
    return true;
}

