heif::Error heif::BoxHeader::prepend_header(StreamWriter& writer, size_t box_start) const
{
  const int reserved_header_size = is_full_box_header() ? (8+4) : 8;


  // determine header size

  int header_size = 0;

  header_size += 8; // normal header size

  if (is_full_box_header()) {
    header_size += 4;
  }

  if (m_type==fourcc("uuid")) {
    header_size += 16;
  }

  bool large_size = false;

  size_t data_size = writer.data_size() - box_start - reserved_header_size;

  if (data_size + header_size > 0xFFFFFFFF) {
    header_size += 8;
    large_size = true;
  }

  size_t box_size = data_size + header_size;


  // --- write header

  writer.set_position(box_start);
  assert(header_size >= reserved_header_size);
  writer.insert(header_size - reserved_header_size);

  if (large_size) {
    writer.write32(1);
  }
  else {
    assert(box_size <= 0xFFFFFFFF);
    writer.write32( (uint32_t)box_size );
  }

  writer.write32( m_type );

  if (large_size) {
    writer.write64( box_size );
  }

  if (m_type==fourcc("uuid")) {
    assert(m_uuid_type.size()==16);
    writer.write(m_uuid_type);
  }

  if (is_full_box_header()) {
    assert((m_flags & ~0x00FFFFFF) == 0);

    writer.write32( (m_version << 24) | m_flags );
  }

  writer.set_position_to_end();  // Note: should we move to the end of the box after writing the header?

  return Error::Ok;
}