# run with:
# source("t2.R")

library(rkafka)

kconf <- rkafka::kafka_conf_new()

rkafka::kafka_conf_set(kconf, "client.id", "another")
rkafka::kafka_conf_set(kconf, "group.id", "1")
rkafka::kafka_conf_set(kconf, "bootstrap.servers", "127.0.0.1:9092")

kc <- rkafka::kafka_consumer_new(kconf)

Sys.sleep(1.0)

topics <- data.frame(topic = c("topic001"))
rkafka::kafka_subscribe(kc, topics)

i <- 0
while (i < 10) {
  res <- rkafka::kafka_consumer_poll(kc, 1000)
  print(paste("i=",i,"value=",res$value,"offset=",res$offset,"partition=",res$partition))
  i <- i + 1
}


if (rkafka::kafka_unsubscribe(kc)) {
  print("successfully unsubscribed")
} else {
  print("failed to unsubscribe")
}

if (rkafka::kafka_consumer_close(kc)) {
  print("successfully closed client")
} else {
  print("failed to close client")
}

