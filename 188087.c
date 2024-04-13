EbmlUnicodeString::EbmlUnicodeString(const EbmlUnicodeString & ElementToClone)
  :EbmlElement(ElementToClone)
  ,Value(ElementToClone.Value)
  ,DefaultValue(ElementToClone.DefaultValue)
{
}