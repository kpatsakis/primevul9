UTFstring & UTFstring::operator=(const wchar_t * _aBuf)
{
  delete [] _Data;
  if (_aBuf == NULL) {
    _Data = new wchar_t[1];
    _Data[0] = 0;
    UpdateFromUCS2();
    return *this;
  }

  size_t aLen;
  for (aLen=0; _aBuf[aLen] != 0; aLen++);
  _Length = aLen;
  _Data = new wchar_t[_Length+1];
  for (aLen=0; _aBuf[aLen] != 0; aLen++) {
    _Data[aLen] = _aBuf[aLen];
  }
  _Data[aLen] = 0;
  UpdateFromUCS2();
  return *this;
}