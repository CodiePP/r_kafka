# rkafka
Interfacing R and Apache Kafka


## build

```sh
R CMD build .
```

this results in a .tar.gz which can then be installed:

```sh
R CMD install rkafka_1.0.tar.gz
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

