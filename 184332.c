void MonClient::send_log(bool flush)
{
  if (log_client) {
    Message *lm = log_client->get_mon_log_message(flush);
    if (lm)
      _send_mon_message(lm);
    more_log_pending = log_client->are_pending();
  }
}