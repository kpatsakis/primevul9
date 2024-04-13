std::string color_profile_nclx::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << indent << "colour_primaries: " << m_colour_primaries << "\n"
       << indent << "transfer_characteristics: " << m_transfer_characteristics << "\n"
       << indent << "matrix_coefficients: " << m_matrix_coefficients << "\n"
       << indent << "full_range_flag: " << m_full_range_flag << "\n";
  return sstr.str();
}