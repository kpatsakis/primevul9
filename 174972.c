std::string Box_auxC::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << Box::dump(indent);

  sstr << indent << "aux type: " << m_aux_type << "\n"
       << indent << "aux subtypes: ";
  for (uint8_t subtype : m_aux_subtypes) {
    sstr << std::hex << std::setw(2) << std::setfill('0') << ((int)subtype) << " ";
  }

  sstr << "\n";

  return sstr.str();
}