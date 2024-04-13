int MonClient::build_initial_monmap()
{
  ldout(cct, 10) << __func__ << dendl;
  int r = monmap.build_initial(cct, false, cerr);
  ldout(cct,10) << "monmap:\n";
  monmap.print(*_dout);
  *_dout << dendl;
  return r;
}