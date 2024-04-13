Error Box_hdlr::parse(BitstreamRange& range)
{
  parse_full_box_header(range);

  m_pre_defined = range.read32();
  m_handler_type = range.read32();

  for (int i=0;i<3;i++) {
    m_reserved[i] = range.read32();
  }

  m_name = range.read_string();

  return range.get_error();
}