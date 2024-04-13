std::string Box_ftyp::dump(Indent& indent) const
{
  std::ostringstream sstr;

  sstr << BoxHeader::dump(indent);

  sstr << indent << "major brand: " << to_fourcc(m_major_brand) << "\n"
       << indent << "minor version: " << m_minor_version << "\n"
       << indent << "compatible brands: ";

  bool first=true;
  for (uint32_t brand : m_compatible_brands) {
    if (first) { first=false; }
    else { sstr << ','; }

    sstr << to_fourcc(brand);
  }
  sstr << "\n";

  return sstr.str();
}