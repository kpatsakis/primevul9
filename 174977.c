size_t heif::BoxHeader::reserve_box_header_space(StreamWriter& writer) const
{
  size_t start_pos = writer.get_position();

  int header_size = is_full_box_header() ? (8+4) : 8;

  writer.skip(header_size);

  return start_pos;
}