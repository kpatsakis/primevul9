int Box_clap::right_rounded(int image_width) const
{
  Fraction pcX  = m_horizontal_offset + Fraction(image_width-1, 2);
  Fraction right = pcX + (m_clean_aperture_width-1)/2;

  return right.round();
}