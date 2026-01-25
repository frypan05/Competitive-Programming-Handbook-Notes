FROM alpine:latest

RUN apk add --no-cache socat && mkdir -p /path/to/directory

EXPOSE 2375

CMD ["socat", "tcp-listen:2375,fork,reuseaddr", "tcp:127.0.0.1:2376"]
