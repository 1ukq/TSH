FROM alpine:latest
RUN apk update
RUN apk add libc-dev
RUN apk add gcc
RUN apk add make
RUN apk add bash
RUN mkdir home/tsh
RUN mkdir home/tsh/src
COPY AUTHORS home/tsh
COPY README.md home/tsh
COPY Makefile home/tsh
COPY src home/tsh/src