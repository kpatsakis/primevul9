void MonClient::flush_log()
{
  std::lock_guard l(monc_lock);
  send_log();
}