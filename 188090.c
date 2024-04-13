const UTFstring & EbmlUnicodeString::DefaultVal() const
{
  assert(DefaultISset());
  return DefaultValue;
}