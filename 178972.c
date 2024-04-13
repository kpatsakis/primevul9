bool RGWCORSRule::has_wildcard_origin() {
  if (allowed_origins.find("*") != allowed_origins.end())
    return true;

  return false;
}