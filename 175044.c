std::string Box_ipco::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << Box::dump(indent);

  sstr << dump_children(indent);

  return sstr.str();
}