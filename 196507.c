std::string QueryParams::getUrl() const
{
  std::string url;
  for (size_type nn = 0; nn < _values.size(); ++nn)
  {
    if (nn > 0)
      url += '&';

    if (!_values[nn].name.empty())
    {
      appendUrl(url, _values[nn].name);
      url += '=';
    }

    appendUrl(url, _values[nn].value);
  }

  return url;
}