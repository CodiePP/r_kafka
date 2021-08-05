
#include <Rcpp.h>
using namespace Rcpp;

#include <librdkafka/rdkafka.h>

// [[Rcpp::export]]
std::string kafka_version() {
  const char *kv = ::rd_kafka_version_str();
  return std::string(kv);
}
