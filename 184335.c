void MonClient::handle_mon_command_ack(MMonCommandAck *ack)
{
  MonCommand *r = NULL;
  uint64_t tid = ack->get_tid();

  if (tid == 0 && !mon_commands.empty()) {
    r = mon_commands.begin()->second;
    ldout(cct, 10) << __func__ << " has tid 0, assuming it is " << r->tid << dendl;
  } else {
    map<uint64_t,MonCommand*>::iterator p = mon_commands.find(tid);
    if (p == mon_commands.end()) {
      ldout(cct, 10) << __func__ << " " << ack->get_tid() << " not found" << dendl;
      ack->put();
      return;
    }
    r = p->second;
  }

  ldout(cct, 10) << __func__ << " " << r->tid << " " << r->cmd << dendl;
  if (r->poutbl)
    r->poutbl->claim(ack->get_data());
  _finish_command(r, ack->r, ack->rs);
  ack->put();
}