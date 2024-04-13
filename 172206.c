static void libopenjpeg_error_callback(const char *msg, void * /*client_data*/) {
  error(errSyntaxError, -1, "{0:s}", msg);
}