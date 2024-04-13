void Box_hvcC::append_nal_data(const std::vector<uint8_t>& nal)
{
  NalArray array;
  array.m_array_completeness = 0;
  array.m_NAL_unit_type = uint8_t(nal[0]>>1);
  array.m_nal_units.push_back(nal);

  m_nal_array.push_back(array);
}