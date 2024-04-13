Error Box_pitm::parse(BitstreamRange& range)
{
  parse_full_box_header(range);

  if (get_version()==0) {
    m_item_ID = range.read16();
  }
  else {
    m_item_ID = range.read32();
  }

  return range.get_error();
}