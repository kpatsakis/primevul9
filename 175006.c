void Box_hvcC::append_nal_data(const uint8_t* data, size_t size)
{
  std::vector<uint8_t> nal;
  nal.resize(size);
  memcpy(nal.data(), data, size);

  NalArray array;
  array.m_array_completeness = 0;
  array.m_NAL_unit_type = uint8_t(nal[0]>>1);
  array.m_nal_units.push_back( std::move(nal) );

  m_nal_array.push_back(array);
}