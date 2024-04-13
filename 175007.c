std::string Box_iinf::dump(Indent& indent ) const
{
  std::ostringstream sstr;
  sstr << Box::dump(indent);

  sstr << dump_children(indent);

  return sstr.str();
}