Error Box_hvcC::parse(BitstreamRange& range)
{
  //parse_full_box_header(range);

  uint8_t byte;

  auto& c = m_configuration; // abbreviation

  c.configuration_version = range.read8();
  byte = range.read8();
  c.general_profile_space = (byte>>6) & 3;
  c.general_tier_flag = (byte>>5) & 1;
  c.general_profile_idc = (byte & 0x1F);

  c.general_profile_compatibility_flags = range.read32();

  for (int i=0; i<6; i++)
    {
      byte = range.read8();

      for (int b=0;b<8;b++) {
        c.general_constraint_indicator_flags[i*8+b] = (byte >> (7-b)) & 1;
      }
    }

  c.general_level_idc = range.read8();
  c.min_spatial_segmentation_idc = range.read16() & 0x0FFF;
  c.parallelism_type = range.read8() & 0x03;
  c.chroma_format = range.read8() & 0x03;
  c.bit_depth_luma = static_cast<uint8_t>((range.read8() & 0x07) + 8);
  c.bit_depth_chroma = static_cast<uint8_t>((range.read8() & 0x07) + 8);
  c.avg_frame_rate = range.read16();

  byte = range.read8();
  c.constant_frame_rate = (byte >> 6) & 0x03;
  c.num_temporal_layers = (byte >> 3) & 0x07;
  c.temporal_id_nested = (byte >> 2) & 1;

  m_length_size = static_cast<uint8_t>((byte & 0x03) + 1);

  int nArrays = range.read8();

  for (int i=0; i<nArrays && !range.error(); i++)
    {
      byte = range.read8();

      NalArray array;

      array.m_array_completeness = (byte >> 6) & 1;
      array.m_NAL_unit_type = (byte & 0x3F);

      int nUnits = range.read16();
      for (int u=0; u<nUnits && !range.error(); u++) {

        std::vector<uint8_t> nal_unit;
        int size = range.read16();
        if (!size) {
          // Ignore empty NAL units.
          continue;
        }

        if (range.prepare_read(size)) {
          nal_unit.resize(size);
          range.get_istream()->read((char*)nal_unit.data(), size);
        }

        array.m_nal_units.push_back( std::move(nal_unit) );
      }

      m_nal_array.push_back( std::move(array) );
    }

  range.skip_to_end_of_box();

  return range.get_error();
}