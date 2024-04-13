std::string Box_clap::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << Box::dump(indent);

  sstr << indent << "clean_aperture: " << m_clean_aperture_width.numerator
       << "/" << m_clean_aperture_width.denominator << " x "
       << m_clean_aperture_height.numerator << "/"
       << m_clean_aperture_height.denominator << "\n";
  sstr << indent << "offset: " << m_horizontal_offset.numerator << "/"
       << m_horizontal_offset.denominator << " ; "
       << m_vertical_offset.numerator << "/"
       << m_vertical_offset.denominator << "\n";

  return sstr.str();
}