Error Box_hdlr::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  writer.write32(m_pre_defined);
  writer.write32(m_handler_type);

  for (int i=0;i<3;i++) {
    writer.write32(m_reserved[i]);
  }

  writer.write(m_name);

  prepend_header(writer, box_start);

  return Error::Ok;
}