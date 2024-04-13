void UTFstring::UpdateFromUCS2()
{
  // find the size of the final UTF-8 string
  size_t i,Size=0;
  for (i=0; i<_Length; i++) {
    if (_Data[i] < 0x80) {
      Size++;
    } else if (_Data[i] < 0x800) {
      Size += 2;
    } else {
      Size += 3;
    }
  }
  std::string::value_type *tmpStr = new std::string::value_type[Size+1];
  for (i=0, Size=0; i<_Length; i++) {
    if (_Data[i] < 0x80) {
      tmpStr[Size++] = _Data[i];
    } else if (_Data[i] < 0x800) {
      tmpStr[Size++] = 0xC0 | (_Data[i] >> 6);
      tmpStr[Size++] = 0x80 | (_Data[i] & 0x3F);
    } else {
      tmpStr[Size++] = 0xE0 | (_Data[i] >> 12);
      tmpStr[Size++] = 0x80 | ((_Data[i] >> 6) & 0x3F);
      tmpStr[Size++] = 0x80 | (_Data[i] & 0x3F);
    }
  }
  tmpStr[Size] = 0;
  UTF8string = tmpStr; // implicit conversion
  delete [] tmpStr;

}