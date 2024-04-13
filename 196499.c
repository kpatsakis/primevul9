void QueryParams::parse_url(const std::string& url)
{
  UrlParser p(*this);

  for (std::string::const_iterator it = url.begin(); it != url.end(); ++it)
    p.parse(*it);

  p.finish();
}