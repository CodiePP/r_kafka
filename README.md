# rkafka
Interfacing R and Apache Kafka

[![compile-R-package](https://github.com/CodiePP/r_kafka/actions/workflows/check-standard.yaml/badge.svg?branch=main)](https://github.com/CodiePP/r_kafka/actions/workflows/check-standard.yaml)

## build

```sh
R CMD build .
```

this results in a .tar.gz which can then be installed:

```sh
R CMD INSTALL rkafka_1.0.tar.gz
```

## testing

```R
library(rkafka)
rkafka::kafka_version()
```

## development

```sh
Rscript prepare.R
R CMD check .
```

