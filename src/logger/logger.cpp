#include "logger.hpp"
#include <glog/logging.h>

void initLogger(const char* argv0, const Config& config) {
    FLAGS_logtostderr = config.log_to_stderr;
    FLAGS_colorlogtostderr = config.color_log;
    FLAGS_minloglevel = config.min_log_level;
    
    google::InitGoogleLogging(argv0);
}