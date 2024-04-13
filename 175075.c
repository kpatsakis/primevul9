Error Box_iloc::parse(BitstreamRange& range)
{
  /*
  printf("box size: %d\n",get_box_size());
  printf("header size: %d\n",get_header_size());
  printf("start limit: %d\n",sizeLimit);
  */

  parse_full_box_header(range);

  uint16_t values4 = range.read16();

  int offset_size = (values4 >> 12) & 0xF;
  int length_size = (values4 >>  8) & 0xF;
  int base_offset_size = (values4 >> 4) & 0xF;
  int index_size = 0;

  if (get_version()>1) {
    index_size = (values4 & 0xF);
  }

  int item_count;
  if (get_version()<2) {
    item_count = range.read16();
  }
  else {
    item_count = range.read32();
  }

  // Sanity check.
  if (item_count > MAX_ILOC_ITEMS) {
    std::stringstream sstr;
    sstr << "iloc box contains " << item_count << " items, which exceeds the security limit of "
         << MAX_ILOC_ITEMS << " items.";

    return Error(heif_error_Memory_allocation_error,
                 heif_suberror_Security_limit_exceeded,
                 sstr.str());
  }

  for (int i=0;i<item_count;i++) {
    Item item;

    if (get_version()<2) {
      item.item_ID = range.read16();
    }
    else {
      item.item_ID = range.read32();
    }

    if (get_version()>=1) {
      values4 = range.read16();
      item.construction_method = (values4 & 0xF);
    }

    item.data_reference_index = range.read16();

    item.base_offset = 0;
    if (base_offset_size==4) {
      item.base_offset = range.read32();
    }
    else if (base_offset_size==8) {
      item.base_offset = ((uint64_t)range.read32()) << 32;
      item.base_offset |= range.read32();
    }

    int extent_count = range.read16();
    // Sanity check.
    if (extent_count > MAX_ILOC_EXTENTS_PER_ITEM) {
      std::stringstream sstr;
      sstr << "Number of extents in iloc box (" << extent_count << ") exceeds security limit ("
           << MAX_ILOC_EXTENTS_PER_ITEM << ")\n";

      return Error(heif_error_Memory_allocation_error,
                   heif_suberror_Security_limit_exceeded,
                   sstr.str());
    }

    for (int e=0;e<extent_count;e++) {
      Extent extent;

      if (get_version()>1 && index_size>0) {
        if (index_size==4) {
          extent.index = range.read32();
        }
        else if (index_size==8) {
          extent.index = ((uint64_t)range.read32()) << 32;
          extent.index |= range.read32();
        }
      }

      extent.offset = 0;
      if (offset_size==4) {
        extent.offset = range.read32();
      }
      else if (offset_size==8) {
        extent.offset = ((uint64_t)range.read32()) << 32;
        extent.offset |= range.read32();
      }


      extent.length = 0;
      if (length_size==4) {
        extent.length = range.read32();
      }
      else if (length_size==8) {
        extent.length = ((uint64_t)range.read32()) << 32;
        extent.length |= range.read32();
      }

      item.extents.push_back(extent);
    }

    if (!range.error()) {
      m_items.push_back(item);
    }
  }

  //printf("end limit: %d\n",sizeLimit);

  return range.get_error();
}