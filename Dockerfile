FROM gcc:9.4
# docker build -t auditlib .
WORKDIR /code
COPY . /code
RUN apt-get update && apt-get install -y build-essential && make
ENV LD_AUDIT=/code/auditlib.so
