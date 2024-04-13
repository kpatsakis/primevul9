EbmlUnicodeString::EbmlUnicodeString(const UTFstring & aDefaultValue)
  :EbmlElement(0, true), Value(aDefaultValue), DefaultValue(aDefaultValue)
{
  SetDefaultSize(0);
  SetDefaultIsSet();
}