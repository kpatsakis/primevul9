Error Box_auxC::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  writer.write(m_aux_type);

  for (uint8_t subtype : m_aux_subtypes) {
    writer.write8(subtype);
  }

  prepend_header(writer, box_start);

  return Error::Ok;
}