Error Box_ispe::parse(BitstreamRange& range)
{
  parse_full_box_header(range);

  m_image_width = range.read32();
  m_image_height = range.read32();

  return range.get_error();
}