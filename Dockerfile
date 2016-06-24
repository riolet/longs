
FROM debian:jessie

RUN apt-get -y update \
	&& apt-get -y install build-essential

# Bundle app source
WORKDIR /var/www/longs
COPY . /var/www/longs
RUN make 

EXPOSE 80
CMD PORT=80 ./longs
