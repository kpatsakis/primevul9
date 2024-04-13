std::string color_profile_raw::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << indent << "profile size: " << m_data.size() << "\n";
  return sstr.str();
}