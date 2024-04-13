Error Box_clap::parse(BitstreamRange& range)
{
  //parse_full_box_header(range);

  m_clean_aperture_width.numerator   = range.read32();
  m_clean_aperture_width.denominator = range.read32();
  m_clean_aperture_height.numerator   = range.read32();
  m_clean_aperture_height.denominator = range.read32();
  m_horizontal_offset.numerator   = range.read32();
  m_horizontal_offset.denominator = range.read32();
  m_vertical_offset.numerator   = range.read32();
  m_vertical_offset.denominator = range.read32();
  if (!m_clean_aperture_width.is_valid() || !m_clean_aperture_height.is_valid() ||
      !m_horizontal_offset.is_valid() || !m_vertical_offset.is_valid()) {
    return Error(heif_error_Invalid_input,
                 heif_suberror_Invalid_fractional_number);
  }

  return range.get_error();
}