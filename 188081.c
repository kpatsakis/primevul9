UTFstring & UTFstring::operator=(wchar_t _aChar)
{
  delete [] _Data;
  _Data = new wchar_t[2];
  _Length = 1;
  _Data[0] = _aChar;
  _Data[1] = 0;
  UpdateFromUCS2();
  return *this;
}