Error Box_ipma::parse(BitstreamRange& range)
{
  parse_full_box_header(range);

  int entry_cnt = range.read32();
  for (int i = 0; i < entry_cnt && !range.error() && !range.eof(); i++) {
    Entry entry;
    if (get_version()<1) {
      entry.item_ID = range.read16();
    }
    else {
      entry.item_ID = range.read32();
    }

    int assoc_cnt = range.read8();
    for (int k=0;k<assoc_cnt;k++) {
      PropertyAssociation association;

      uint16_t index;
      if (get_flags() & 1) {
        index = range.read16();
        association.essential = !!(index & 0x8000);
        association.property_index = (index & 0x7fff);
      }
      else {
        index = range.read8();
        association.essential = !!(index & 0x80);
        association.property_index = (index & 0x7f);
      }

      entry.associations.push_back(association);
    }

    m_entries.push_back(entry);
  }

  return range.get_error();
}