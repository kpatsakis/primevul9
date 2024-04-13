Error Box_ftyp::parse(BitstreamRange& range)
{
  m_major_brand = range.read32();
  m_minor_version = range.read32();

  if (get_box_size() <= get_header_size() + 8) {
    // Sanity check.
    return Error(heif_error_Invalid_input,
                 heif_suberror_Invalid_box_size,
                 "ftyp box too small (less than 8 bytes)");
  }

  uint64_t n_minor_brands = (get_box_size() - get_header_size() - 8) / 4;

  for (uint64_t i=0;i<n_minor_brands && !range.error();i++) {
    m_compatible_brands.push_back( range.read32() );
  }

  return range.get_error();
}