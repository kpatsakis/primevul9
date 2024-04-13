void Box_infe::derive_box_version()
{
  int min_version = 0;

  if (m_hidden_item) {
    min_version = std::max(min_version, 2);
  }

  if (m_item_ID > 0xFFFF) {
    min_version = std::max(min_version, 3);
  }


  if (m_item_type != "") {
    min_version = std::max(min_version, 2);
  }

  set_version((uint8_t)min_version);
}