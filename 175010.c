void Box_iloc::patch_iloc_header(StreamWriter& writer) const
{
  size_t old_pos = writer.get_position();
  writer.set_position(m_iloc_box_start);

  writer.write8((uint8_t)((m_offset_size<<4) | (m_length_size)));
  writer.write8((uint8_t)((m_base_offset_size<<4) | (m_index_size)));

  if (get_version() < 2) {
    writer.write16((uint16_t)m_items.size());
  } else {
    writer.write32((uint32_t)m_items.size());
  }

  for (const auto& item : m_items) {
    if (get_version() < 2) {
      writer.write16((uint16_t)item.item_ID);
    } else {
      writer.write32((uint32_t)item.item_ID);
    }

    if (get_version() >= 1) {
      writer.write16(item.construction_method);
    }

    writer.write16(item.data_reference_index);
    writer.write(m_base_offset_size, item.base_offset);
    writer.write16((uint16_t)item.extents.size());

    for (const auto& extent : item.extents) {
      if (get_version()>=1 && m_index_size > 0) {
        writer.write(m_index_size, extent.index);
      }

      writer.write(m_offset_size, extent.offset);
      writer.write(m_length_size, extent.length);
    }
  }

  writer.set_position(old_pos);
}