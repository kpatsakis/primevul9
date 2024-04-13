Error Box_iloc::append_data(heif_item_id item_ID,
                            const std::vector<uint8_t>& data,
                            uint8_t construction_method)
{
  // check whether this item ID already exists

  size_t idx;
  for (idx=0;idx<m_items.size();idx++) {
    if (m_items[idx].item_ID == item_ID) {
      break;
    }
  }

  // item does not exist -> add a new one to the end

  if (idx == m_items.size()) {
    Item item;
    item.item_ID = item_ID;
    item.construction_method = construction_method;

    m_items.push_back(item);
  }

  if (m_items[idx].construction_method != construction_method) {
    // TODO: return error: construction methods do not match
  }

  Extent extent;
  extent.data = data;
  m_items[idx].extents.push_back( std::move(extent) );

  return Error::Ok;
}