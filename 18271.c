void LogHandler::rolloutHandler(const char *filename, std::size_t size) {
  // SHOULD NOT LOG ANYTHING HERE BECAUSE LOG FILE IS CLOSED!
  // REMOVE OLD LOG
  remove(filename);
}