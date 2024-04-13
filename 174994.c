void Box::derive_box_version_recursive()
{
  derive_box_version();

  for (auto& child : m_children) {
    child->derive_box_version_recursive();
  }
}