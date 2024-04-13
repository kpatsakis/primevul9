local_scan_crash_handler(int sig)
{
log_write(0, LOG_MAIN|LOG_REJECT, "local_scan() function crashed with "
  "signal %d - message temporarily rejected (size %d)", sig, message_size);
/* Does not return */
receive_bomb_out(US"local-scan-error", US"local verification problem");
}