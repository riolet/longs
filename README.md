# Lon.gs
[![Build Status](https://jenkins-longs.hotbed.io/buildStatus/icon?job=longs-longs)](https://jenkins-longs.hotbed.io/job/longs-longs/)

Welcome to Lon.gs, the url shortner.

Lon.gs is built upon [WAFer](https://github.com/riolet/WAFer), a light flyweight platform for creating lightning fast and scalable network applications using the C language.

Lon.gs is built with [SQLite](https://www.sqlite.org/), a self-contained, embeddable, zero-configuration SQL database engine, so no database setup is necessary.

Lon.gs will happily do 10K concurrent connections on an AWS t2.nano, where our website is being served from.

You can configure the app using the following environmental variables:

| Option | Defualt Value |
|--------|---------|
| PORT   | 4242    |
| BASE_URL   | lon.gs    |
| DB_PATH   | URL_Records.db    |
| HASH_LENGTH   | 5    |



## Docker

You may use our official image on [Docker Hub](https://hub.docker.com/r/riolet/longs/).

```bash
docker docker pull riolet/longs
docker run -p 80:80 riolet/longs
```


This repo includes a Dockerfile, which allows it to be built into a docker container.

```bash
docker build --tag=longs .
docker run -p 80:80 longs
```

This will start Lon.gs listening on port 80 on your Docker net IP
