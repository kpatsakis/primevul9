int Box_clap::get_height_rounded() const
{
  int top    = (Fraction(0,1)-(m_clean_aperture_height-1)/2).round();
  int bottom = ( (m_clean_aperture_height-1)/2).round();

  return bottom+1-top;
}