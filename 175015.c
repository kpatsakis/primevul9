void Box_pitm::derive_box_version()
{
  if (m_item_ID <= 0xFFFF) {
    set_version(0);
  }
  else {
    set_version(1);
  }
}