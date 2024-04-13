void MonClient::_finish_command(MonCommand *r, int ret, string rs)
{
  ldout(cct, 10) << __func__ << " " << r->tid << " = " << ret << " " << rs << dendl;
  if (r->prval)
    *(r->prval) = ret;
  if (r->prs)
    *(r->prs) = rs;
  if (r->onfinish)
    finisher.queue(r->onfinish, ret);
  mon_commands.erase(r->tid);
  delete r;
}