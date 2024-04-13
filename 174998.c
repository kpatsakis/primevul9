int Box_clap::left_rounded(int image_width) const
{
  // pcX = horizOff + (width  - 1)/2
  // pcX ± (cleanApertureWidth - 1)/2

  // left = horizOff + (width-1)/2 - (clapWidth-1)/2

  Fraction pcX  = m_horizontal_offset + Fraction(image_width-1, 2);
  Fraction left = pcX - (m_clean_aperture_width-1)/2;

  return left.round();
}