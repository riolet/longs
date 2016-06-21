/*
 * background-image.h
 *
 *  Created on: Jul 19, 2014
 *      Author: rohana
 */

#ifndef STYLES_H_
#define STYLES_H_

/*Some ENTL Extensions*/
#define QCONSOLE(text) QTAGA(div,class="console",QTAG(PRE,text))
#define QCODE(text) QTAGA(div,class="code",QTAG(PRE,text))

#define QEM(text)  QTAG(em,text)
#define QSTRONG(text) QTAG(strong,text)
#define QITM(text) QTAG(li,text)

#define MVHP_OPEN(l,c,t,h)  LT(!DOCTYPE html) GT CRLF\
							LT(html) STR(lang=l) GT CRLF\
									OTAG(head)\
										STAG(meta, charset=c) CRLF\
										QTAG(title,t) CRLF\
										h CRLF\
									CTAG(head) CRLF\
									OTAG(body) CRLF

#define MVHPEN8(t,h) MVHP_OPEN("en","utf-8",t,h)

#define HP_CLOSE CRLF CTAG(body) CRLF CTAG(html)

/*End Extensions*/

#define HEADER_LAYOUT \
                MVHPEN8("%s" /*title*/,\
                                        STAG(meta, property="og:image" content="http://nopedotc.com/ogimage.png")\
                                        STAG(meta, property=viewport content="width=device-width, initial-scale=1")\
                                        OTAG(style) MAIN_CSS SINGLE_COLUMN_CSS CTAG(style)\
                        )\
                        OTAGA(div,id="container")\
                                OTAGA(div, id="header")\
                                        QTAG(h1,QLINKA("/",style="text-decoration:none;color:black","Lon.gs"))\
                                CTAG(div)\
                                OTAGA(div, id="content-container")\
                                OTAGA(div, id="content")


#define FOOTER_LAYOUT  "\n\t\t<div id=\"footer\">\r\n\t\t\tCopyright \u00A9 Riolet Corporation, %d\r\n\t\t</div>\r\n\t</div>\r\n</div>" HP_CLOSE

#define WELCOME_PAGE \
                CTAG(div)\
				OTAGA(div,id="content")\
				QTAG(h2, "Welcome to Lon.gs, the url shortner.")\
				QTAG(P, "Lon.gs is built upon " QLINK("https://github.com/riolet/WAFer","WAFer")  ", a light flyweight platform for creating lightning fast and scalable network applications using C language.")\
				QTAG(P, "Lon.gs is built with " QLINK("https://www.sqlite.org/","SQLite") ", a self-contained, embeddable, zero-configuration SQL database engine, so no database setup is necessary.")\
				QTAG(P, "Lon.gs will happily do 10K concurrent connections on an AWS t2.nano, where this website is being served from.")\
				QTAG(P, "Lon.gs is open source, check us out on " QLINK("https://github.com/riolet/longs","github"))\
				CTAG(div)



#include "background_image.h"

#define MAIN_CSS "#container{margin:0 auto;width:100%;background:#fff}"\
	"#header{background:#ccc;padding:20px}"\
	"#header h1{margin:0}"\
	"#navigation{float:left;width:100%;background:#333}"\
	"#navigation ul{margin:0;padding:0}"\
	"#navigation ul li{list-style-type:none;display:inline}"\
	"#navigation li a{display:block;float:left;padding:5px 10px;color:#fff;text-decoration:none;border-right:1px solid #fff}"\
	"#navigation li a:hover{background:#383}"\
	"#content-container{float:left;width:100%;background:#FFF url(" BG_IMAGE ") repeat-y 68% 0}"\
	"#content{clear:left;float:left;width:60%;padding:20px 0;margin:0 0 0 4%;display:inline}"\
	"#content h2{margin:0}"\
	"#aside{float:right;width:26%;padding:20px 0;margin:0 3%;display:inline}"\
	"#aside h3{margin:0}"\
	"#footer{clear:left;background:#ccc;text-align:right;padding:20px;height:1%}"\
	"div.console{padding:.5em;clear:both;overflow:auto;background:#000;color:#FFF;font-family:\"Courier New\",Courier,monospace}"\
	"div.code{border:1px solid #DCDCDC;padding:1em;overflow:auto;background:#FFF}"\
	"@media only screen and (device-width: 360px) and (device-height: 640px) and (orientation: portrait) {"\
		"#container{margin:0;padding:0;font-size: 2em;}"\
		"#content-container{float:left;width:100%;background:#FFF}"\
		"#aside,#content{clear:both;width:100%;margin:0;display:inline}"\
		"div.code{max-height:800px}"\
		"#footer{clear:both;text-align:right}}"\
	"@media only screen and (device-width: 720px) and (device-height: 1280px) and (orientation: portrait) {"\
		"#container{margin:0;padding:0;font-size: 2em;}"\
		"#content-container{float:left;width:100%;background:#FFF}"\
		"#aside,#content{clear:both;width:100%;margin:0;display:inline}"\
		"div.code{max-height:800px}"\
		"#footer{clear:both;text-align:right}}"\
	"@media only screen and (max-width:767px) {"\
		"#container{margin:0;padding:0;font-size: large;font-size: 2em;}"\
		"#content-container{float:left;width:100%;background:#FFF}"\
		"#aside,#content{clear:both;width:100%;margin:0;display:inline}"\
		"div.code{max-height:400px}"\
		"#footer{clear:both;text-align:right}}"\
	".comment { color: #999999; font-style: italic; }"\
	".pre { color: #000099; }\r\n.string { color: #009900; }"\
	".char { color: #009900; }\r\n.float { color: #996600; }"\
	".int { color: #999900; }\r\n.bool { color: #000000; font-weight: bold; }"\
	".type { color: #FF6633; }\r\n.flow { color: #FF0000; }\r\n.keyword { color: #990000; }"\
	".operator { color: #663300; font-weight: bold; }"

#define SINGLE_COLUMN_CSS "#content{clear:left;float:left;width:90%;padding:20px 0;margin:0 0 0 4%;display:inline}"\
        "#content-container{float:left;width:100%;background:#FFF}"

#endif /* STYLES_H_*/
