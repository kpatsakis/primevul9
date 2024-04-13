std::string Box_hvcC::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << Box::dump(indent);

  const auto& c = m_configuration; // abbreviation

  sstr << indent << "configuration_version: " << ((int)c.configuration_version) << "\n"
       << indent << "general_profile_space: " << ((int)c.general_profile_space) << "\n"
       << indent << "general_tier_flag: " << c.general_tier_flag << "\n"
       << indent << "general_profile_idc: " << ((int)c.general_profile_idc) << "\n";

  sstr << indent << "general_profile_compatibility_flags: ";
  for (int i=0;i<32;i++) {
    sstr << ((c.general_profile_compatibility_flags>>(31-i))&1);
    if ((i%8)==7) sstr << ' ';
    else if ((i%4)==3) sstr << '.';
  }
  sstr << "\n";

  sstr << indent << "general_constraint_indicator_flags: ";
  int cnt=0;
  for (int i=0; i<configuration::NUM_CONSTRAINT_INDICATOR_FLAGS; i++) {
    bool b = c.general_constraint_indicator_flags[i];

    sstr << (b ? 1:0);
    cnt++;
    if ((cnt%8)==0)
      sstr << ' ';
  }
  sstr << "\n";

  sstr << indent << "general_level_idc: " << ((int)c.general_level_idc) << "\n"
       << indent << "min_spatial_segmentation_idc: " << c.min_spatial_segmentation_idc << "\n"
       << indent << "parallelism_type: " << ((int)c.parallelism_type) << "\n"
       << indent << "chroma_format: " << ((int)c.chroma_format) << "\n"
       << indent << "bit_depth_luma: " << ((int)c.bit_depth_luma) << "\n"
       << indent << "bit_depth_chroma: " << ((int)c.bit_depth_chroma) << "\n"
       << indent << "avg_frame_rate: " << c.avg_frame_rate << "\n"
       << indent << "constant_frame_rate: " << ((int)c.constant_frame_rate) << "\n"
       << indent << "num_temporal_layers: " << ((int)c.num_temporal_layers) << "\n"
       << indent << "temporal_id_nested: " << ((int)c.temporal_id_nested) << "\n"
       << indent << "length_size: " << ((int)m_length_size) << "\n";

  for (const auto& array : m_nal_array) {
    sstr << indent << "<array>\n";

    indent++;
    sstr << indent << "array_completeness: " << ((int)array.m_array_completeness) << "\n"
         << indent << "NAL_unit_type: " << ((int)array.m_NAL_unit_type) << "\n";

    for (const auto& unit : array.m_nal_units) {
      //sstr << "  unit with " << unit.size() << " bytes of data\n";
      sstr << indent;
      for (uint8_t b : unit) {
        sstr << std::setfill('0') << std::setw(2) << std::hex << ((int)b) << " ";
      }
      sstr << "\n";
      sstr << std::dec;
    }

    indent--;
  }

  return sstr.str();
}