void Box_iref::derive_box_version()
{
  uint8_t version = 0;

  for (const auto& ref : m_references) {
    if (ref.from_item_ID > 0xFFFF) {
      version=1;
      break;
    }

    for (uint32_t r : ref.to_item_ID) {
      if (r > 0xFFFF) {
        version=1;
        break;
      }
    }
  }

  set_version(version);
}