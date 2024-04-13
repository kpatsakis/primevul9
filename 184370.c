void MonClient::handle_monmap(MMonMap *m)
{
  ldout(cct, 10) << __func__ << " " << *m << dendl;
  auto con_addrs = m->get_source_addrs();
  string old_name = monmap.get_name(con_addrs);

  // NOTE: we're not paying attention to the epoch, here.
  auto p = m->monmapbl.cbegin();
  decode(monmap, p);

  ldout(cct, 10) << " got monmap " << monmap.epoch
		 << " from mon." << old_name
		 << " (according to old e" << monmap.get_epoch() << ")"
 		 << dendl;
  ldout(cct, 10) << "dump:\n";
  monmap.print(*_dout);
  *_dout << dendl;

  if (old_name.size() == 0) {
    ldout(cct,10) << " can't identify which mon we were connected to" << dendl;
    _reopen_session();
  } else {
    int new_rank = monmap.get_rank(m->get_source_addr());
    if (new_rank < 0) {
      ldout(cct, 10) << "mon." << new_rank << " at " << m->get_source_addrs()
		     << " went away" << dendl;
      // can't find the mon we were talking to (above)
      _reopen_session();
    } else if (messenger->should_use_msgr2() &&
	       monmap.get_addrs(new_rank).has_msgr2() &&
	       !con_addrs.has_msgr2()) {
      ldout(cct,1) << " mon." << new_rank << " has (v2) addrs "
		   << monmap.get_addrs(new_rank) << " but i'm connected to "
		   << con_addrs << ", reconnecting" << dendl;
      _reopen_session();
    }
  }

  cct->set_mon_addrs(monmap);

  sub.got("monmap", monmap.get_epoch());
  map_cond.Signal();
  want_monmap = false;

  if (authenticate_err == 1) {
    _finish_auth(0);
  }
}