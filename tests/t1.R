# run with:
# source("t1.R")

library(rkafka)

kconf <- rkafka::kafka_conf_new()

rkafka::kafka_conf_set(kconf, "client.id", "it-s-me")
rkafka::kafka_conf_set(kconf, "bootstrap.servers", "localhost:9092")

kp <- rkafka::kafka_producer_new(kconf)

tconf <- rkafka::kafka_topic_conf_new()
rkafka::kafka_topic_conf_set(tconf, "acks", "all")
ktopic <- rkafka::kafka_topic_new(kp, "topic001", tconf)

rkafka::kafka_produce(ktopic, 0, "hello there", "")
rkafka::kafka_produce(ktopic, -1, "hello all", "")
msgs <- data.frame(value = c("one", "two", "three", "four", "five", "six"))
rkafka::kafka_produce_batch(ktopic, -1, msgs)

rkafka::kafka_flush(kp, 500)
Sys.sleep(1)

rkafka::kafka_destroy(kp)
#rkafka::kafka_topic_destroy(ktopic)
#rkafka::kafka_topic_conf_destroy(tconf)
#rkafka::kafka_conf_destroy(kconf)


