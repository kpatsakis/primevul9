static void libopenjpeg_warning_callback(const char *msg, void * /*client_data*/) {
  error(errSyntaxWarning, -1, "{0:s}", msg);
}