bool Box_iref::has_references(uint32_t itemID) const
{
  for (const Reference& ref : m_references) {
    if (ref.from_item_ID == itemID) {
      return true;
    }
  }

  return false;
}