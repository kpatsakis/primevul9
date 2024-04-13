Error Box_url::parse(BitstreamRange& range)
{
  parse_full_box_header(range);

  m_location = range.read_string();

  return range.get_error();
}