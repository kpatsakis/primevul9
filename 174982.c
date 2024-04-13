const std::vector<Box_ipma::PropertyAssociation>* Box_ipma::get_properties_for_item_ID(uint32_t itemID) const
{
  for (const auto& entry : m_entries) {
    if (entry.item_ID == itemID) {
      return &entry.associations;
    }
  }

  return nullptr;
}