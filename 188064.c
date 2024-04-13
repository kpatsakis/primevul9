void EbmlUnicodeString::SetDefaultValue(UTFstring & aValue)
{
  assert(!DefaultISset());
  DefaultValue = aValue;
  SetDefaultIsSet();
}