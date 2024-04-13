int Box_clap::bottom_rounded(int image_height) const
{
  Fraction pcY  = m_vertical_offset + Fraction(image_height-1, 2);
  Fraction bottom = pcY + (m_clean_aperture_height-1)/2;

  return bottom.round();
}