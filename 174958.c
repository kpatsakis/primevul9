Error Box_colr::parse(BitstreamRange& range)
{
  StreamReader::grow_status status;
  uint32_t colour_type = range.read32();

  if (colour_type == fourcc("nclx")) {
    auto color_profile = std::make_shared<color_profile_nclx>();
    m_color_profile = color_profile;
    Error err = color_profile->parse(range);
    if (err) {
      return err;
    }
  } else if (colour_type == fourcc("prof") ||
             colour_type == fourcc("rICC")) {
    auto profile_size = get_box_size() - get_header_size() - 4;
    status = range.wait_for_available_bytes(profile_size);
    if (status != StreamReader::size_reached) {
      // TODO: return recoverable error at timeout
      return Error(heif_error_Invalid_input,
                   heif_suberror_End_of_data);
    }

    std::vector<uint8_t> rawData(profile_size);
    for (size_t i = 0; i < profile_size; i++ ){
      rawData[i] = range.read8();
    }

    m_color_profile = std::make_shared<color_profile_raw>(colour_type, rawData);
  }
  else {
    return Error(heif_error_Invalid_input,
                 heif_suberror_Unknown_color_profile_type);
  }

  return range.get_error();
}