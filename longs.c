#include "nope.h"
#include "nopeapi.h"
#include <string.h>

static void form(Request *request, Response *response);
static void shorten (char *longUrl, char *shortUrl);
static void getLongUrl (char *shortUrl, char *longUrl);

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
    strcpy(shortUrl,"hello");
}

static void getLongUrl (char *shortUrl, char *longUrl) {
    //Long URL fetching logic goes here
    strcpy(longUrl,"https://github.com/riolet");
}
