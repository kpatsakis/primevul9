Error Box_iloc::write_mdat_after_iloc(StreamWriter& writer)
{
  // --- compute sum of all mdat data

  size_t sum_mdat_size = 0;

  for (const auto& item : m_items) {
    if (item.construction_method == 0) {
      for (const auto& extent : item.extents) {
        sum_mdat_size += extent.data.size();
      }
    }
  }

  if (sum_mdat_size > 0xFFFFFFFF) {
    // TODO: box size > 4 GB
  }


  // --- write mdat box

  writer.write32((uint32_t)(sum_mdat_size + 8));
  writer.write32(fourcc("mdat"));

  for (auto& item : m_items) {
    item.base_offset = writer.get_position();

    for (auto& extent : item.extents) {
      extent.offset = writer.get_position() - item.base_offset;
      extent.length = extent.data.size();

      writer.write(extent.data);
    }
  }


  // --- patch iloc box

  patch_iloc_header(writer);

  return Error::Ok;
}