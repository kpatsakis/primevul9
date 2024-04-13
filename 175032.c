std::vector<std::shared_ptr<Box>> Box::get_child_boxes(uint32_t short_type) const
{
  std::vector<std::shared_ptr<Box>> result;
  for (auto& box : m_children) {
    if (box->get_short_type()==short_type) {
      result.push_back(box);
    }
  }

  return result;
}