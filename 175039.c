void Box_iloc::derive_box_version()
{
  int min_version = m_user_defined_min_version;

  if (m_items.size() > 0xFFFF) {
    min_version = std::max(min_version, 2);
  }

  m_offset_size = 0;
  m_length_size = 0;
  m_base_offset_size = 0;
  m_index_size = 0;

  for (const auto& item : m_items) {
    // check item_ID size
    if (item.item_ID > 0xFFFF) {
      min_version = std::max(min_version, 2);
    }

    // check construction method
    if (item.construction_method != 0) {
      min_version = std::max(min_version, 1);
    }

    // base offset size
    /*
    if (item.base_offset > 0xFFFFFFFF) {
      m_base_offset_size = 8;
    }
    else if (item.base_offset > 0) {
      m_base_offset_size = 4;
    }
    */

    /*
    for (const auto& extent : item.extents) {
      // extent index size

      if (extent.index != 0) {
        min_version = std::max(min_version, 1);
        m_index_size = 4;
      }

      if (extent.index > 0xFFFFFFFF) {
        m_index_size = 8;
      }

      // extent offset size
      if (extent.offset > 0xFFFFFFFF) {
        m_offset_size = 8;
      }
      else {
        m_offset_size = 4;
      }

      // extent length size
      if (extent.length > 0xFFFFFFFF) {
        m_length_size = 8;
      }
      else {
        m_length_size = 4;
      }
    }
      */
  }

  m_offset_size = 4;
  m_length_size = 4;
  m_base_offset_size = 4; // TODO: or could be 8 if we write >4GB files
  m_index_size = 0;

  set_version((uint8_t)min_version);
}