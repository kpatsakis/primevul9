Error Box::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  Error err = write_children(writer);

  prepend_header(writer, box_start);

  return err;
}