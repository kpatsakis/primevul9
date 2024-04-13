bool Box_hvcC::get_headers(std::vector<uint8_t>* dest) const
{
  for (const auto& array : m_nal_array) {
    for (const auto& unit : array.m_nal_units) {

      dest->push_back( (unit.size()>>24) & 0xFF );
      dest->push_back( (unit.size()>>16) & 0xFF );
      dest->push_back( (unit.size()>> 8) & 0xFF );
      dest->push_back( (unit.size()>> 0) & 0xFF );

      /*
      dest->push_back(0);
      dest->push_back(0);
      dest->push_back(1);
      */

      dest->insert(dest->end(), unit.begin(), unit.end());
    }
  }

  return true;
}