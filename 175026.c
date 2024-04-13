std::vector<uint32_t> Box_iref::get_references(uint32_t itemID, uint32_t ref_type) const
{
  for (const Reference& ref : m_references) {
    if (ref.from_item_ID == itemID &&
        ref.header.get_short_type() == ref_type) {
      return ref.to_item_ID;
    }
  }

  return std::vector<uint32_t>();
}