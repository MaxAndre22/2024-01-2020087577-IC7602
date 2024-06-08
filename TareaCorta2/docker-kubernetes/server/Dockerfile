FROM gcc:4.9
WORKDIR /app
COPY app/. .
RUN gcc -std=c99 -o appc app.c
CMD ["./appc"]
EXPOSE 9666
