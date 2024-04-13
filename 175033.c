std::string BoxHeader::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << indent << "Box: " << get_type_string() << " -----\n";
  sstr << indent << "size: " << get_box_size() << "   (header size: " << get_header_size() << ")\n";

  if (m_is_full_box) {
    sstr << indent << "version: " << ((int)m_version) << "\n"
         << indent << "flags: " << std::hex << m_flags << "\n";
  }

  return sstr.str();
}