Error Box_auxC::parse(BitstreamRange& range)
{
  parse_full_box_header(range);

  m_aux_type = range.read_string();

  while (!range.eof()) {
    m_aux_subtypes.push_back( range.read8() );
  }

  return range.get_error();
}