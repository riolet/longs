FROM debian:jessie

RUN apt-get -y update \
	&& apt-get -y install build-essential

# Bundle app source
WORKDIR /var/www/longs
COPY . /var/www/longs
RUN make 
RUN mkdir /data && chown :0 /data && chmod 775 /data && chmod g+s /data
ENV DB_PATH=/data/URL_Records.db
EXPOSE 8080
CMD PORT=8080 ./longs
