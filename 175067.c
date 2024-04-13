Error Box_grpl::parse(BitstreamRange& range)
{
  //parse_full_box_header(range);

  //return read_children(range);

  while (!range.eof()) {
    EntityGroup group;
    Error err = group.header.parse(range);
    if (err != Error::Ok) {
      return err;
    }

    err = group.header.parse_full_box_header(range);
    if (err != Error::Ok) {
      return err;
    }

    group.group_id = range.read32();
    int nEntities = range.read32();
    for (int i=0;i<nEntities;i++) {
      if (range.eof()) {
        break;
      }

      group.entity_ids.push_back( range.read32() );
    }

    m_entity_groups.push_back(group);
  }

  return range.get_error();
}