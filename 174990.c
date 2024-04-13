int Box_clap::get_width_rounded() const
{
  int left  = (Fraction(0,1)-(m_clean_aperture_width-1)/2).round();
  int right = (  (m_clean_aperture_width-1)/2).round();

  return right+1-left;
}