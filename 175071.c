Error Box_pixi::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  if (m_bits_per_channel.size() > 255 ||
      m_bits_per_channel.empty()) {
    // TODO: error
    assert(false);
  }

  writer.write8((uint8_t)(m_bits_per_channel.size()));
  for (size_t i=0;i<m_bits_per_channel.size();i++) {
    writer.write8(m_bits_per_channel[i]);
  }

  prepend_header(writer, box_start);

  return Error::Ok;
}