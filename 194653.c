data_sigterm_sigint_handler(int sig)
{
uschar *msg = NULL;

if (smtp_input)
  {
  msg = US"Service not available - SIGTERM or SIGINT received";
  log_write(0, LOG_MAIN, "%s closed after %s", smtp_get_connection_info(),
    (sig == SIGTERM)? "SIGTERM" : "SIGINT");
  }
else
  {
  if (filter_test == FTEST_NONE)
    {
    fprintf(stderr, "\nexim: %s received - message abandoned\n",
      (sig == SIGTERM)? "SIGTERM" : "SIGINT");
    log_write(0, LOG_MAIN, "%s received while reading local message",
      (sig == SIGTERM)? "SIGTERM" : "SIGINT");
    }
  }

receive_bomb_out(US"signal-exit", msg);    /* Does not return */
}