  ~sst_logger_thread_arg() {
    if (err_pipe) fclose(err_pipe);
    err_pipe = NULL;
  }