FROM kstmorozov/gateway_server:latest
MAINTAINER Konstantin Morozov <morozov-kst@yandex.ru>
ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /server

ADD ./gateway-server /server

CMD ["/server/gateway-server"]

EXPOSE 7777
EXPOSE 5432