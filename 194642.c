handle_lost_connection(uschar *s)
{
log_write(L_lost_incoming_connection | L_smtp_connection, LOG_MAIN,
  "%s lost while reading message data%s", smtp_get_connection_info(), s);
smtp_notquit_exit(US"connection-lost", NULL, NULL);
return US"421 Lost incoming connection";
}