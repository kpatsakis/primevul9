Error Box_iprp::parse(BitstreamRange& range)
{
  //parse_full_box_header(range);

  return read_children(range);
}