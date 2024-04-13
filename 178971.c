void RGWCORSRule::erase_origin_if_present(string& origin, bool *rule_empty) {
  set<string>::iterator it = allowed_origins.find(origin);
  if (!rule_empty)
    return;
  *rule_empty = false;
  if (it != allowed_origins.end()) {
    dout(10) << "Found origin " << origin << ", set size:" << 
        allowed_origins.size() << dendl;
    allowed_origins.erase(it);
    *rule_empty = (allowed_origins.empty());
  }
}