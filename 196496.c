void QueryParams::parse_url(std::istream& url_stream)
{
  UrlParser p(*this);

  char ch;
  while (url_stream.get(ch))
    p.parse(ch);

  p.finish();
}