# Lon.gs

Welcome to Lon.gs, the url shortner.

Lon.gs is built upon [WAFer](https://github.com/riolet/WAFer), a light flyweight platform for creating lightning fast and scalable network applications using the C language.

Lon.gs is built with [SQLite](https://www.sqlite.org/), a self-contained, embeddable, zero-configuration SQL database engine, so no database setup is necessary.

Lon.gs will happily do 10K concurrent connections on an AWS t2.nano, where our website is being served from.

Default port is 4242. Set environment variable 'PORT' to change it.
