bool RGWCORSRule::is_header_allowed(const char *h, size_t len) {
  string hdr(h, len);
  if(lowercase_allowed_hdrs.empty()) {
    set<string>::iterator iter;
    for (iter = allowed_hdrs.begin(); iter != allowed_hdrs.end(); ++iter) {
      lowercase_allowed_hdrs.insert(lowercase_http_attr(*iter));
    }
  }
  return is_string_in_set(lowercase_allowed_hdrs, lowercase_http_attr(hdr));
}