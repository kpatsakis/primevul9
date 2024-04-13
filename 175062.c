Error Box_ipma::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  size_t entry_cnt = m_entries.size();
  writer.write32((uint32_t)entry_cnt);

  for (const Entry& entry : m_entries) {

    if (get_version()<1) {
      writer.write16((uint16_t)entry.item_ID);
    }
    else {
      writer.write32(entry.item_ID);
    }

    size_t assoc_cnt = entry.associations.size();
    if (assoc_cnt > 0xFF) {
      // TODO: error, too many associations
    }

    writer.write8((uint8_t)assoc_cnt);

    for (const PropertyAssociation& association : entry.associations) {

      if (get_flags() & 1) {
        writer.write16( (uint16_t)((association.essential ? 0x8000 : 0) |
                                   (association.property_index & 0x7FFF)) );
      }
      else {
        writer.write8( (uint8_t)((association.essential ? 0x80 : 0) |
                                 (association.property_index & 0x7F)) );
      }
    }
  }

  prepend_header(writer, box_start);

  return Error::Ok;
}