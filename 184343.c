void MonClient::start_mon_command(const string &mon_name,
				 const vector<string>& cmd,
				 const bufferlist& inbl,
				 bufferlist *outbl, string *outs,
				 Context *onfinish)
{
  ldout(cct,10) << __func__ << " mon." << mon_name << " cmd=" << cmd << dendl;
  std::lock_guard l(monc_lock);
  if (!initialized || stopping) {
    if (onfinish) {
      onfinish->complete(-ECANCELED);
    }
    return;
  }
  MonCommand *r = new MonCommand(++last_mon_command_tid);
  r->target_name = mon_name;
  r->cmd = cmd;
  r->inbl = inbl;
  r->poutbl = outbl;
  r->prs = outs;
  r->onfinish = onfinish;
  mon_commands[r->tid] = r;
  _send_command(r);
}