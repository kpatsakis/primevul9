Error Box_pitm::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  if (get_version()==0) {
    assert(m_item_ID <= 0xFFFF);
    writer.write16((uint16_t)m_item_ID);
  }
  else {
    writer.write32(m_item_ID);
  }

  prepend_header(writer, box_start);

  return Error::Ok;
}