std::string Box_idat::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << Box::dump(indent);

  sstr << indent << "number of data bytes: " << get_box_size() - get_header_size() << "\n";

  return sstr.str();
}