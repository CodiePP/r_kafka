
#include <Rcpp.h>
using namespace Rcpp;

#include <utility>
#include <algorithm>

#include <librdkafka/rdkafka.h>

// [[Rcpp::export]]
std::string kafka_version() {
  const char *kv = ::rd_kafka_version_str();
  return std::string(kv);
}

// [[Rcpp::export]]
uint64_t kafka_conf_new() {
    rd_kafka_conf_t *kc = rd_kafka_conf_new();
    return (uint64_t)kc;
}

// [[Rcpp::export]]
bool kafka_conf_set(uint64_t p, std::string const &k, std::string const &v) {
    rd_kafka_conf_t *kc = (rd_kafka_conf_t*)p;
    char errstr[128];
    rd_kafka_conf_res_t res = rd_kafka_conf_set(kc, k.c_str(), k.c_str(), errstr, 127);
    if (res != RD_KAFKA_CONF_OK) {
        std::clog << "ERROR: kafka_conf_set returned: " << errstr << std::endl;
        return false;
    }
    return true;
}

// [[Rcpp::export]]
void kafka_conf_destroy(uint64_t p) {
    rd_kafka_conf_t *kc = (rd_kafka_conf_t*)p;
    rd_kafka_conf_destroy(kc);
}

// [[Rcpp::export]]
uint64_t kafka_topic_conf_new() {
    rd_kafka_topic_conf_t *kc = rd_kafka_topic_conf_new();
    return (uint64_t)kc;
}

// [[Rcpp::export]]
bool kafka_topic_conf_set(uint64_t p, std::string const &k, std::string const &v) {
    rd_kafka_topic_conf_t *kc = (rd_kafka_topic_conf_t*)p;
    char errstr[128];
    rd_kafka_conf_res_t res = rd_kafka_topic_conf_set(kc, k.c_str(), v.c_str(), errstr, 127);
    if (res != RD_KAFKA_CONF_OK) {
        std::clog << "ERROR: kafka_topic_conf_set returned: " << errstr << std::endl;
        return false;
    }
    return true;
}

// [[Rcpp::export]]
void kafka_topic_conf_destroy(uint64_t p) {
    rd_kafka_topic_conf_t *kc = (rd_kafka_topic_conf_t*)p;
    rd_kafka_topic_conf_destroy(kc);
}

uint64_t kafka_new(std::string const &name, int ty, uint64_t p_conf) {
    rd_kafka_conf_t *kc = (rd_kafka_conf_t*)p_conf;
    char errstr[128];
    rd_kafka_t *k = rd_kafka_new((rd_kafka_type_t)ty, kc, errstr, 127);
    if (!k) {
        std::clog << "ERROR: kafka_new " << name << " returned: " << errstr << std::endl;
        return 0;
    }
    return (uint64_t)k;
}

// [[Rcpp::export]]
uint64_t kafka_consumer_new(uint64_t p_conf) {
    return kafka_new("consumer", RD_KAFKA_CONSUMER, p_conf);
}

// [[Rcpp::export]]
uint64_t kafka_producer_new(uint64_t p_conf) {
    return kafka_new("producer", RD_KAFKA_PRODUCER, p_conf);
}

// [[Rcpp::export]]
void kafka_destroy(uint64_t p) {
    rd_kafka_t *k = (rd_kafka_t*)p;
    rd_kafka_destroy(k);
}

// [[Rcpp::export]]
uint64_t kafka_topic_new(uint64_t p_client, std::string const &name, uint64_t p_conf) {
    rd_kafka_t *k = (rd_kafka_t*)p_client;
    rd_kafka_topic_conf_t *kc = (rd_kafka_topic_conf_t*)p_conf;
    rd_kafka_topic_t *t = rd_kafka_topic_new(k, name.c_str(), kc);
    return (uint64_t)t;
}

// [[Rcpp::export]]
void kafka_topic_destroy(uint64_t p) {
    rd_kafka_topic_t *t = (rd_kafka_topic_t*)p;
    rd_kafka_topic_destroy(t);
}

// [[Rcpp::export]]
bool kafka_produce(uint64_t p_topic, int partition, std::string const &payload, std::string const &key) {
    rd_kafka_topic_t *t = (rd_kafka_topic_t*)p_topic;
    // a negative partition indicates "unassigned" partition
    if (partition < 0) { partition = RD_KAFKA_PARTITION_UA; }
    const char *k = key.empty() ? NULL : key.c_str();
    int k_sz = k == NULL ? 0 : key.length();
    int res = rd_kafka_produce(t, partition,
                  // TODO is this the right way?
                  RD_KAFKA_MSG_F_COPY | RD_KAFKA_MSG_F_BLOCK,
                  (void*)payload.c_str(), payload.length(),
                  (void*)k, k_sz,
                  NULL);
    return (res == 0);
}

// [[Rcpp::export]]
bool kafka_produce_batch(uint64_t p_topic, int partition, DataFrame batch) {
    rd_kafka_topic_t *t = (rd_kafka_topic_t*)p_topic;
    // a negative partition indicates "unassigned" partition
    if (partition < 0) { partition = RD_KAFKA_PARTITION_UA; }
    // prepare list of messages
    StringVector ks;
    if (batch.containsElementNamed("key"))
        ks = batch["key"];
    StringVector vs;
    if (batch.containsElementNamed("value")) {
        vs = batch["value"];
    } else {
        return false;
    }
    rd_kafka_message_t msgs[vs.size()];
    int idx = 0;
    for (auto const &v : vs) {
        char *p = NULL; int pn = 0;
        p = v; pn = strlen(p);
        char *k = NULL; int kn = 0;
        if (idx < ks.size()) { k = ks[idx]; kn = strlen(k); }
        msgs[idx].payload = (void*)p;
        msgs[idx].len = pn;
        msgs[idx].key = (void*)k;
        msgs[idx].key_len = kn;
        idx++;
    }
    int res = rd_kafka_produce_batch(t, partition,
                  // TODO is this the right way?
                  RD_KAFKA_MSG_F_COPY | RD_KAFKA_MSG_F_BLOCK,
                  msgs, idx);
    return (res == 0);
}

// [[Rcpp::export]]
StringVector kafka_consume_batch(uint64_t p_topic, int partition, int32_t timeout) {
    rd_kafka_topic_t *t = (rd_kafka_topic_t*)p_topic;
    // a negative partition indicates "unassigned" partition
    if (partition < 0) { partition = RD_KAFKA_PARTITION_UA; }
    int sz = 100;
    rd_kafka_message_t* msgs[sz];

    int res = rd_kafka_consume_batch(t, partition, timeout,
                                   msgs, sz);
    if (res <= 0) {
        return false;
    }

    StringVector vs;
    for (int idx = 0; idx < res; idx++) {
        vs.push_back(std::string((char*)msgs[idx]->payload, msgs[idx]->len));
    }
    return vs;    
}

// [[Rcpp::export]]
bool kafka_consume_start(uint64_t p_topic, int partition, int64_t offset) {
    rd_kafka_topic_t *t = (rd_kafka_topic_t*)p_topic;
    // a negative partition indicates "unassigned" partition
    if (partition < 0) { partition = RD_KAFKA_PARTITION_UA; }

    int res = rd_kafka_consume_start(t, partition, offset);
    return (res == 0);
}

// [[Rcpp::export]]
bool kafka_consume_stop(uint64_t p_topic, int partition) {
    rd_kafka_topic_t *t = (rd_kafka_topic_t*)p_topic;
    // a negative partition indicates "unassigned" partition
    if (partition < 0) { partition = RD_KAFKA_PARTITION_UA; }

    int res = rd_kafka_consume_stop(t, partition);
    return (res == 0);
}

// [[Rcpp::export]]
bool kafka_flush(uint64_t p_client, int32_t timeout) {
    rd_kafka_t *k = (rd_kafka_t*)p_client;

    rd_kafka_resp_err_t res = rd_kafka_flush(k, timeout);
    return (res == RD_KAFKA_RESP_ERR_NO_ERROR);
}

// [[Rcpp::export]]
List kafka_consumer_poll(uint64_t p_client, int32_t timeout) {
    rd_kafka_t *k = (rd_kafka_t*)p_client;

    rd_kafka_message_t *msg = rd_kafka_consumer_poll(k, timeout);

    StringVector value;
    value.push_back(std::string((const char*)msg->payload, msg->len));
    NumericVector offset;
    offset.push_back(msg->offset);
    NumericVector partition;
    partition.push_back(msg->partition);
    List res = List::create(Named("value") = value,
                            Named("offset") = offset,
                            Named("partition") = partition);
    if (msg->key && msg->key_len > 0) {
        StringVector key;
        key.push_back(std::string((const char*)msg->key, msg->key_len));
        res["key"] = key;
    }
    return res;
}

// [[Rcpp::export]]
bool kafka_consumer_close(uint64_t p_client) {
    rd_kafka_t *k = (rd_kafka_t*)p_client;

    rd_kafka_resp_err_t res = rd_kafka_consumer_close(k);
    return (res == RD_KAFKA_RESP_ERR_NO_ERROR);
}