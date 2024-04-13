bool RGWCORSRule::is_origin_present(const char *o) {
  string origin = o;
  return is_string_in_set(allowed_origins, origin);
}