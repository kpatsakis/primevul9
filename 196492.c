bool QueryParams::has(const std::string& name) const
{
  for (size_type nn = 0; nn < _values.size(); ++nn)
    if (_values[nn].name == name)
      return true;

  return false;
}