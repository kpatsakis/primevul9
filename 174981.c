Error Box_iinf::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  int nEntries_size = (get_version() > 0) ? 4 : 2;

  writer.write(nEntries_size, m_children.size());


  Error err = write_children(writer);

  prepend_header(writer, box_start);

  return err;
}