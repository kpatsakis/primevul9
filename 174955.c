Error Box_hvcC::write(StreamWriter& writer) const
{
  size_t box_start = reserve_box_header_space(writer);

  const auto& c = m_configuration; // abbreviation

  writer.write8(c.configuration_version);

  writer.write8((uint8_t)(((c.general_profile_space & 3) << 6) |
                          ((c.general_tier_flag & 1) << 5) |
                          (c.general_profile_idc & 0x1F)));

  writer.write32(c.general_profile_compatibility_flags);

  for (int i=0; i<6; i++)
    {
      uint8_t byte = 0;

      for (int b=0;b<8;b++) {
        if (c.general_constraint_indicator_flags[i*8+b]) {
          byte |= 1;
        }

        byte = (uint8_t)(byte<<1);
      }

      writer.write8(byte);
    }

  writer.write8(c.general_level_idc);
  writer.write16((c.min_spatial_segmentation_idc & 0x0FFF) | 0xF000);
  writer.write8(c.parallelism_type | 0xFC);
  writer.write8(c.chroma_format | 0xFC);
  writer.write8((uint8_t)((c.bit_depth_luma - 8) | 0xF8));
  writer.write8((uint8_t)((c.bit_depth_chroma - 8) | 0xF8));
  writer.write16(c.avg_frame_rate);

  writer.write8((uint8_t)(((c.constant_frame_rate & 0x03) << 6) |
                          ((c.num_temporal_layers & 0x07) << 3) |
                          ((c.temporal_id_nested & 1) << 2) |
                          ((m_length_size-1) & 0x03)));

  size_t nArrays = m_nal_array.size();
  if (nArrays>0xFF) {
    // TODO: error: too many NAL units
  }

  writer.write8((uint8_t)nArrays);

  for (const NalArray& array : m_nal_array) {

    writer.write8((uint8_t)(((array.m_array_completeness & 1) << 6) |
                            (array.m_NAL_unit_type & 0x3F)));

    size_t nUnits = array.m_nal_units.size();
    if (nUnits > 0xFFFF) {
      // TODO: error: too many NAL units
    }

    writer.write16((uint16_t)nUnits);

    for (const std::vector<uint8_t>& nal_unit : array.m_nal_units) {
      writer.write16((uint16_t)nal_unit.size());
      writer.write(nal_unit);
    }
  }

  prepend_header(writer, box_start);

  return Error::Ok;
}