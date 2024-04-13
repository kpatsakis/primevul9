Error Box_ispe::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  writer.write32(m_image_width);
  writer.write32(m_image_height);

  prepend_header(writer, box_start);

  return Error::Ok;
}