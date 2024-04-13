void Box_iref::add_reference(heif_item_id from_id, uint32_t type, std::vector<heif_item_id> to_ids)
{
  Reference ref;
  ref.header.set_short_type(type);
  ref.from_item_ID = from_id;
  ref.to_item_ID = to_ids;

  m_references.push_back(ref);
}