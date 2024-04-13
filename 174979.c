void Box_infe::set_hidden_item(bool hidden)
{
  m_hidden_item = hidden;

  if (m_hidden_item) {
    set_flags( get_flags() | 1);
  }
  else {
    set_flags( get_flags() & ~1);
  }
}