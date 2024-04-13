void Box_ipma::add_property_for_item_ID(heif_item_id itemID,
                                        PropertyAssociation assoc)
{
  size_t idx;
  for (idx=0; idx<m_entries.size(); idx++) {
    if (m_entries[idx].item_ID == itemID) {
      break;
    }
  }

  // if itemID does not exist, add a new entry
  if (idx == m_entries.size()) {
    Entry entry;
    entry.item_ID = itemID;
    m_entries.push_back(entry);
  }

  // add the property association
  m_entries[idx].associations.push_back(assoc);
}