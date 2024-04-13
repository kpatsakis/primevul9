Error Box_meta::parse(BitstreamRange& range)
{
  parse_full_box_header(range);

  /*
  uint64_t boxSizeLimit;
  if (get_box_size() == BoxHeader::size_until_end_of_file) {
    boxSizeLimit = sizeLimit;
  }
  else {
    boxSizeLimit = get_box_size() - get_header_size();
  }
  */

  return read_children(range);
}