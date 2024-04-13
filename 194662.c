local_scan_timeout_handler(int sig)
{
sig = sig;    /* Keep picky compilers happy */
log_write(0, LOG_MAIN|LOG_REJECT, "local_scan() function timed out - "
  "message temporarily rejected (size %d)", message_size);
/* Does not return */
receive_bomb_out(US"local-scan-timeout", US"local verification problem");
}