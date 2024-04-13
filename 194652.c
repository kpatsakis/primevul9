data_timeout_handler(int sig)
{
uschar *msg = NULL;

sig = sig;    /* Keep picky compilers happy */

if (smtp_input)
  {
  msg = US"SMTP incoming data timeout";
  log_write(L_lost_incoming_connection,
            LOG_MAIN, "SMTP data timeout (message abandoned) on connection "
            "from %s F=<%s>",
            (sender_fullhost != NULL)? sender_fullhost : US"local process",
            sender_address);
  }
else
  {
  fprintf(stderr, "exim: timed out while reading - message abandoned\n");
  log_write(L_lost_incoming_connection,
            LOG_MAIN, "timed out while reading local message");
  }

receive_bomb_out(US"data-timeout", msg);   /* Does not return */
}