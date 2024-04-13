EbmlUnicodeString &EbmlUnicodeString::SetValueUTF8(std::string const &NewValue) {
  UTFstring NewValueUTFstring;
  NewValueUTFstring.SetUTF8(NewValue);
  return *this = NewValueUTFstring;
}