Error BoxHeader::parse_full_box_header(BitstreamRange& range)
{
  uint32_t data = range.read32();
  m_version = static_cast<uint8_t>(data >> 24);
  m_flags = data & 0x00FFFFFF;
  m_is_full_box = true;

  m_header_size += 4;

  return range.get_error();
}