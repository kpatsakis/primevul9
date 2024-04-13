Error Box_iref::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  int id_size = ((get_version()==0) ? 2 : 4);

  for (const auto& ref : m_references) {
    uint32_t box_size = uint32_t(4+4 + 2 + id_size * (1+ref.to_item_ID.size()));

    // we write the BoxHeader ourselves since it is very simple
    writer.write32(box_size);
    writer.write32(ref.header.get_short_type());

    writer.write(id_size, ref.from_item_ID);
    writer.write16((uint16_t)ref.to_item_ID.size());

    for (uint32_t r : ref.to_item_ID) {
      writer.write(id_size, r);
    }
  }


  prepend_header(writer, box_start);

  return Error::Ok;
}