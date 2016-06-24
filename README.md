# Lon.gs

[![Build Status](https://drone.io/github.com/riolet/longs/status.png)](https://drone.io/github.com/riolet/longs/latest)

Welcome to Lon.gs, the url shortner.

Lon.gs is built upon [WAFer](https://github.com/riolet/WAFer), a light flyweight platform for creating lightning fast and scalable network applications using the C language.

Lon.gs is built with [SQLite](https://www.sqlite.org/), a self-contained, embeddable, zero-configuration SQL database engine, so no database setup is necessary.

Lon.gs will happily do 10K concurrent connections on an AWS t2.nano, where our website is being served from.

You can configure the app using the following environmental variables:

| Option | Defualt Value |
|--------|---------|
| PORT   | 8242    |
| BASE_URL   | lon.gs    |
| DB_PATH   | URL_Records.db    |
| HASH_LENGTH   | 5    |
