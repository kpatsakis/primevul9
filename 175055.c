bool Box_ftyp::has_compatible_brand(uint32_t brand) const
{
  for (uint32_t b : m_compatible_brands) {
    if (b==brand) {
      return true;
    }
  }

  return false;
}