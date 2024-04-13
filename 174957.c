std::shared_ptr<Box> Box_ipco::get_property_for_item_ID(heif_item_id itemID,
                                                        const std::shared_ptr<class Box_ipma>& ipma,
                                                        uint32_t box_type) const
{
  const std::vector<Box_ipma::PropertyAssociation>* property_assoc = ipma->get_properties_for_item_ID(itemID);
  if (property_assoc == nullptr) {
    return nullptr;
  }

  auto allProperties = get_all_child_boxes();
  for (const  Box_ipma::PropertyAssociation& assoc : *property_assoc) {
    if (assoc.property_index > allProperties.size() ||
        assoc.property_index == 0) {
      return nullptr;
    }

    auto property = allProperties[assoc.property_index - 1];
    if (property->get_short_type() == box_type) {
      return property;
    }
  }

  return nullptr;
}