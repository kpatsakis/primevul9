RGWCORSRule * RGWCORSConfiguration::host_name_rule(const char *origin) {
  for(list<RGWCORSRule>::iterator it_r = rules.begin(); 
      it_r != rules.end(); ++it_r) {
    RGWCORSRule& r = (*it_r);
    if (r.is_origin_present(origin))
      return &r;
  }
  return NULL;
}