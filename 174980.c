std::vector<uint8_t> heif::BoxHeader::get_type() const
{
  if (m_type == fourcc("uuid")) {
    return m_uuid_type;
  }
  else {
    std::vector<uint8_t> type(4);
    type[0] = static_cast<uint8_t>((m_type>>24) & 0xFF);
    type[1] = static_cast<uint8_t>((m_type>>16) & 0xFF);
    type[2] = static_cast<uint8_t>((m_type>> 8) & 0xFF);
    type[3] = static_cast<uint8_t>((m_type>> 0) & 0xFF);
    return type;
  }
}