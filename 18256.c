string LogHandler::stderrToFile(const string &pathPrefix) {
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d_%I-%M", timeinfo);
  string current_time(buffer);
  string stderrFilename = pathPrefix + "_stderr_" + current_time;
  FILE *stderr_stream = freopen(stderrFilename.c_str(), "w", stderr);
  fs::permissions(
      stderrFilename,
      fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read,
      fs::perm_options::replace);
  if (!stderr_stream) {
    STFATAL << "Invalid filename " << stderrFilename;
  }
  setvbuf(stderr_stream, NULL, _IOLBF, BUFSIZ);  // set to line buffering
  return stderrFilename;
}