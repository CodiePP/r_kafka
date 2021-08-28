# r_kafka
Interfacing R and Apache Kafka


## build

```sh
R CMD build rkafka
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
cd rkafka
Rscript prepare.R
cd ..
R CMD check rkafka
```

