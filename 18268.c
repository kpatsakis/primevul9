void LogHandler::setupStdoutLogger() {
  el::Logger *stdoutLogger = el::Loggers::getLogger("stdout");
  // Easylogging configurations
  el::Configurations stdoutConf;
  stdoutConf.setToDefault();
  // Values are always std::string
  stdoutConf.setGlobally(el::ConfigurationType::Format, "%msg");
  stdoutConf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
  stdoutConf.setGlobally(el::ConfigurationType::ToFile, "false");
  el::Loggers::reconfigureLogger(stdoutLogger, stdoutConf);
}