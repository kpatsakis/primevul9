void RGWCORSRule::dump_origins() {
  unsigned num_origins = allowed_origins.size();
  dout(10) << "Allowed origins : " << num_origins << dendl;
  for(set<string>::iterator it = allowed_origins.begin();
      it != allowed_origins.end(); 
      ++it) {
    dout(10) << *it << "," << dendl;
  }
}