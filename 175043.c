std::string Box_colr::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << Box::dump(indent);

  sstr << indent << "colour_type: " << to_fourcc(get_color_profile_type()) << "\n";

  if (m_color_profile) {
    sstr << m_color_profile->dump(indent);
  }
  else {
    sstr << "no color profile\n";
  }

  return sstr.str();
}