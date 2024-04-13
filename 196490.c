std::string QueryParams::param(const std::string& name, size_type n, const std::string& def) const
{
  for (size_type nn = 0; nn < _values.size(); ++nn)
  {
    if (_values[nn].name == name)
    {
      if (n == 0)
        return _values[nn].value;
      --n;
    }
  }

  return def;
}