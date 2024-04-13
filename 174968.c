Error Box_iloc::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  m_iloc_box_start = writer.get_position();

  int nSkip = 0;

  nSkip += 2;
  nSkip += (get_version()<2) ? 2 : 4; // item_count

  for (const auto& item : m_items) {
    nSkip += (get_version()<2) ? 2 : 4; // item_ID
    nSkip += (get_version()>=1) ? 2 : 0; // construction method
    nSkip += 4 + m_base_offset_size;

    for (const auto& extent : item.extents) {
      (void)extent;

      if (get_version()>=1) {
        nSkip += m_index_size;
      }

      nSkip += m_offset_size + m_length_size;
    }
  }

  writer.skip(nSkip);
  prepend_header(writer, box_start);

  return Error::Ok;
}