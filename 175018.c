Error Box_pixi::parse(BitstreamRange& range)
{
  parse_full_box_header(range);

  StreamReader::grow_status status;
  uint8_t num_channels = range.read8();
  status = range.wait_for_available_bytes(num_channels);
  if (status != StreamReader::size_reached) {
    // TODO: return recoverable error at timeout
    return Error(heif_error_Invalid_input,
                 heif_suberror_End_of_data);
  }

  m_bits_per_channel.resize(num_channels);
  for (int i=0;i<num_channels;i++) {
    m_bits_per_channel[i] = range.read8();
  }

  return range.get_error();
}