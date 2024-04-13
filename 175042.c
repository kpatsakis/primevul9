std::string Box::dump(Indent& indent ) const
{
  std::ostringstream sstr;

  sstr << BoxHeader::dump(indent);

  return sstr.str();
}