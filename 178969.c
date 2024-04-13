void RGWCORSConfiguration::erase_host_name_rule(string& origin) {
  bool rule_empty;
  unsigned loop = 0;
  /*Erase the host name from that rule*/
  dout(10) << "Num of rules : " << rules.size() << dendl;
  for(list<RGWCORSRule>::iterator it_r = rules.begin(); 
      it_r != rules.end(); ++it_r, loop++) {
    RGWCORSRule& r = (*it_r);
    r.erase_origin_if_present(origin, &rule_empty);
    dout(10) << "Origin:" << origin << ", rule num:" 
      << loop << ", emptying now:" << rule_empty << dendl;
    if (rule_empty) {
      rules.erase(it_r);
      break;
    }
  }
}