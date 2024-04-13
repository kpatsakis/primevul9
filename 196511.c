QueryParams::size_type QueryParams::paramcount(const std::string& name) const
{
  size_type count = 0;

  for (size_type nn = 0; nn < _values.size(); ++nn)
    if (_values[nn].name == name)
      ++count;

  return count;
}