Error Box_ftyp::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  writer.write32(m_major_brand);
  writer.write32(m_minor_version);

  for (uint32_t b : m_compatible_brands) {
    writer.write32(b);
  }

  prepend_header(writer, box_start);

  return Error::Ok;
}