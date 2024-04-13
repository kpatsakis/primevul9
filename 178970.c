void RGWCORSConfiguration::dump() {
  unsigned loop = 1;
  unsigned num_rules = rules.size();
  dout(10) << "Number of rules: " << num_rules << dendl;
  for(list<RGWCORSRule>::iterator it = rules.begin();
      it!= rules.end(); ++it, loop++) {
    dout(10) << " <<<<<<< Rule " << loop << " >>>>>>> " << dendl;
    (*it).dump_origins();
  }
}