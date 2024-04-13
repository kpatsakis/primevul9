heif::Error heif::BoxHeader::parse(BitstreamRange& range)
{
  StreamReader::grow_status status;
  status = range.wait_for_available_bytes(8);
  if (status != StreamReader::size_reached) {
    // TODO: return recoverable error at timeout
    return Error(heif_error_Invalid_input,
                 heif_suberror_End_of_data);
  }

  m_size = range.read32();
  m_type = range.read32();

  m_header_size = 8;

  if (m_size==1) {
    status = range.wait_for_available_bytes(8);
    if (status != StreamReader::size_reached) {
      // TODO: return recoverable error at timeout
      return Error(heif_error_Invalid_input,
                   heif_suberror_End_of_data);
    }

    uint64_t high = range.read32();
    uint64_t low  = range.read32();

    m_size = (high<<32) | low;
    m_header_size += 8;

    std::stringstream sstr;
    sstr << "Box size " << m_size << " exceeds security limit.";

    if (m_size > MAX_LARGE_BOX_SIZE) {
      return Error(heif_error_Memory_allocation_error,
                   heif_suberror_Security_limit_exceeded,
                   sstr.str());
    }
  }

  if (m_type==fourcc("uuid")) {
    status = range.wait_for_available_bytes(16);
    if (status != StreamReader::size_reached) {
      // TODO: return recoverable error at timeout
      return Error(heif_error_Invalid_input,
                   heif_suberror_End_of_data);
    }

    if (range.prepare_read(16)) {
      m_uuid_type.resize(16);
      range.get_istream()->read((char*)m_uuid_type.data(), 16);
    }

    m_header_size += 16;
  }

  return range.get_error();
}