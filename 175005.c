Error Box_irot::parse(BitstreamRange& range)
{
  //parse_full_box_header(range);

  uint16_t rotation = range.read8();
  rotation &= 0x03;

  m_rotation = rotation * 90;

  return range.get_error();
}