void LogHandler::setupLogFile(el::Configurations *defaultConf, string filename,
                              string maxlogsize) {
  // Enable strict log file size check
  el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
  defaultConf->setGlobally(el::ConfigurationType::Filename, filename);
  defaultConf->setGlobally(el::ConfigurationType::ToFile, "true");
  defaultConf->setGlobally(el::ConfigurationType::MaxLogFileSize, maxlogsize);
}