Error Box_iinf::parse(BitstreamRange& range)
{
  parse_full_box_header(range);

  int nEntries_size = (get_version() > 0) ? 4 : 2;

  int item_count;
  if (nEntries_size==2) {
    item_count = range.read16();
  }
  else {
    item_count = range.read32();
  }

  if (item_count == 0) {
    return Error::Ok;
  }

  // TODO: Only try to read "item_count" children.
  return read_children(range);
}