EbmlUnicodeString & EbmlUnicodeString::operator=(const UTFstring & NewString)
{
  Value = NewString;
  SetValueIsSet();
  return *this;
}