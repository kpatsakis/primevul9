Error Box_colr::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  assert(m_color_profile);

  writer.write32(m_color_profile->get_type());

  Error err = m_color_profile->write(writer);
  if (err) {
    return err;
  }

  prepend_header(writer, box_start);

  return Error::Ok;
}