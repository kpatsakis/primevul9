static void *sst_logger_thread(void *a) {
  sst_logger_thread_arg *arg = (sst_logger_thread_arg *)a;

#ifdef HAVE_PSI_INTERFACE
  wsrep_pfs_register_thread(key_THREAD_wsrep_sst_logger);
#endif /* HAVE_PSI_INTERFACE */

  int const out_len = 1024;
  char out_buf[out_len];

  /* Loops over the READ end of stderr.  Only stops
     when the pipe is in an error or EOF state.
   */
  while (!feof(arg->err_pipe) && !ferror(arg->err_pipe)) {
    char *p = my_fgets(out_buf, out_len, arg->err_pipe);
    if (!p) continue;

    enum loglevel level = string_to_loglevel(p);
    if (level != SYSTEM_LEVEL) {
      WSREP_SST_LOG(level, p + 4);
      if (level == ERROR_LEVEL) {
        flush_error_log_messages();
        pxc_force_flush_error_message = true;
      }
    } else if (strncmp(p, "FIL:", 4) == 0) {
      /* Expect a string with 3 components (separated by semi-colons)
          "FIL" marker
          Level marker : "ERR", "WRN", "INF", "DBG"
          Descriptive string
      */
      std::string s;
      std::string description;
      s.reserve(8002);

      level = string_to_loglevel(p + 4);
      if (level == SYSTEM_LEVEL) {
        // Unknown priority level
        WSREP_ERROR("Unexpected line formatting: %s", p);
        level = INFORMATION_LEVEL;
        description = "(Unknown)";
      } else
        description = p + 8;

      // Header line
      s = "------------ " + description + " (START) ------------\n";

      // Loop around until we get an "EOF:"
      while ((p = fgets(out_buf, out_len, arg->err_pipe)) != NULL) {
        if (strncmp(p, "EOF:", 4) == 0) break;

        // Output the data  if we have to (data max is 8K)
        if (s.length() + strlen(p) + 1 >= 8000) {
          // Ensure the string ends with a newline
          if (s.back() != '\n') s += "\n";
          WSREP_SST_LOG(level, s.c_str());
          s = "------------ " + description + " (cont) ------------\n";
        }
        s += "\t";  // Indent the file contents
        s += p;
      }

      // Either we've had an error, or we've gotten to the end
      // Either way, dump all of our contents
      WSREP_SST_LOG(level, s.c_str());

      s = "------------ " + description + " (END) ------------";
      WSREP_SST_LOG(level, s.c_str());
    } else {
      // Unexpected output
      WSREP_SST_LOG(INFORMATION_LEVEL, p);
    }
  }

#ifdef HAVE_PSI_INTERFACE
  wsrep_pfs_delete_thread();
#endif /* HAVE_PSI_INTERFACE */

  return NULL;
}