Error Box_idat::parse(BitstreamRange& range)
{
  //parse_full_box_header(range);

  m_data_start_pos = range.get_istream()->get_position();

  return range.get_error();
}