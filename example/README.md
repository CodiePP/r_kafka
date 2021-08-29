
# example of writer and reader on Kafka topic

## prereqs

have a Kafka server running and update the code with its hostname and port.
choose a topic name and enter in the code.

## writer

the writer sends every second a new data point to the Kafka channel.

```sh
Rscript example/writer.R
```

## reader

the reader listens on the channel and adds the new data point to a dataframe and plots this one.

```sh
Rscript example/reader.R
```

