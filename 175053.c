std::shared_ptr<Box> Box::get_child_box(uint32_t short_type) const
{
  for (auto& box : m_children) {
    if (box->get_short_type()==short_type) {
      return box;
    }
  }

  return nullptr;
}