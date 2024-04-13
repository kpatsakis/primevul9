void QueryParams::parse_url(const char* url)
{
  UrlParser p(*this);

  while (*url)
  {
    p.parse(*url);
    ++url;
  }

  p.finish();
}