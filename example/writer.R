# R Kafka interface
# example writer
# Copyright 2021 A. Diemand

library(rkafka)

# settings
KHOST <- "localhost"
KPORT <- "9092"
TOPIC <- "channel001"

kconf <- rkafka::kafka_conf_new()

rkafka::kafka_conf_set(kconf, "client.id", "me-writer")
rkafka::kafka_conf_set(kconf, "bootstrap.servers", paste0(KHOST,":",KPORT))

kp <- rkafka::kafka_producer_new(kconf)

tconf <- rkafka::kafka_topic_conf_new()
rkafka::kafka_topic_conf_set(tconf, "acks", "all")
ktopic <- rkafka::kafka_topic_new(kp, TOPIC, tconf)

print(paste("continuously writing to topic",TOPIC))
print("(press Ctrl-C to abort)")
v <- 1.0
while (TRUE) {
  r <- rnorm(1, mean = 0, sd = 0.2)
  rkafka::kafka_produce(ktopic, 0, sprintf("%1.3f", v*(1+r)), "")
  rkafka::kafka_flush(kp, 500)
  Sys.sleep(1)
}

rkafka::kafka_destroy(kp)
