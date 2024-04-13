Error Box_infe::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  if (get_version() <= 1) {
    writer.write16((uint16_t)m_item_ID);
    writer.write16(m_item_protection_index);

    writer.write(m_item_name);
    writer.write(m_content_type);
    writer.write(m_content_encoding);
  }

  if (get_version() >= 2) {
    if (get_version() == 2) {
      writer.write16((uint16_t)m_item_ID);
    }
    else if (get_version()==3) {
      writer.write32(m_item_ID);
    }

    writer.write16(m_item_protection_index);

    if (m_item_type.empty()) {
      writer.write32(0);
    }
    else {
      writer.write32(from_fourcc(m_item_type.c_str()));
    }

    writer.write(m_item_name);
    if (m_item_type == "mime") {
      writer.write(m_content_type);
      writer.write(m_content_encoding);
    }
    else if (m_item_type == "uri ") {
      writer.write(m_item_uri_type);
    }
  }

  prepend_header(writer, box_start);

  return Error::Ok;
}