UTFstring::UTFstring(const wchar_t * _aBuf)
  :_Length(0)
  ,_Data(NULL)
{
  *this = _aBuf;
}