Error Box_imir::parse(BitstreamRange& range)
{
  //parse_full_box_header(range);

  uint16_t axis = range.read8();
  if (axis & 1) {
    m_axis = MirrorAxis::Horizontal;
  }
  else {
    m_axis = MirrorAxis::Vertical;
  }

  return range.get_error();
}