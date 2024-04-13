void Box_ipma::derive_box_version()
{
  int version = 0;
  bool large_property_indices = false;

  for (const Entry& entry : m_entries) {
    if (entry.item_ID > 0xFFFF) {
      version = 1;
    }

    for (const auto& assoc : entry.associations) {
      if (assoc.property_index > 0x7F) {
        large_property_indices=true;
      }
    }
  }

  set_version((uint8_t)version);
  set_flags(large_property_indices ? 1 : 0);
}