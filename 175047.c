std::vector<Box_iref::Reference> Box_iref::get_references_from(heif_item_id itemID) const
{
  std::vector<Reference> references;

  for (const Reference& ref : m_references) {
    if (ref.from_item_ID == itemID) {
      references.push_back(ref);
    }
  }

  return references;
}