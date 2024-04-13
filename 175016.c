Error color_profile_nclx::parse(BitstreamRange& range)
{
  StreamReader::grow_status status;
  status = range.wait_for_available_bytes(7);
  if (status != StreamReader::size_reached) {
    // TODO: return recoverable error at timeout
    return Error(heif_error_Invalid_input,
                 heif_suberror_End_of_data);
  }

  m_colour_primaries = range.read16();
  m_transfer_characteristics = range.read16();
  m_matrix_coefficients = range.read16();
  m_full_range_flag = (range.read8() & 0x80 ? true : false);

  return Error::Ok;
}