void Box_iinf::derive_box_version()
{
  if (m_children.size() > 0xFFFF) {
    set_version(1);
  }
  else {
    set_version(0);
  }
}