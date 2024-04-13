void MonClient::start_mon_command(const vector<string>& cmd,
				 const bufferlist& inbl,
				 bufferlist *outbl, string *outs,
				 Context *onfinish)
{
  ldout(cct,10) << __func__ << " cmd=" << cmd << dendl;
  std::lock_guard l(monc_lock);
  if (!initialized || stopping) {
    if (onfinish) {
      onfinish->complete(-ECANCELED);
    }
    return;
  }
  MonCommand *r = new MonCommand(++last_mon_command_tid);
  r->cmd = cmd;
  r->inbl = inbl;
  r->poutbl = outbl;
  r->prs = outs;
  r->onfinish = onfinish;
  if (cct->_conf->rados_mon_op_timeout > 0) {
    class C_CancelMonCommand : public Context
    {
      uint64_t tid;
      MonClient *monc;
      public:
      C_CancelMonCommand(uint64_t tid, MonClient *monc) : tid(tid), monc(monc) {}
      void finish(int r) override {
	monc->_cancel_mon_command(tid);
      }
    };
    r->ontimeout = new C_CancelMonCommand(r->tid, this);
    timer.add_event_after(cct->_conf->rados_mon_op_timeout, r->ontimeout);
  }
  mon_commands[r->tid] = r;
  _send_command(r);
}