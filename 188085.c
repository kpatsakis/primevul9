UTFstring::UTFstring(const UTFstring & _aBuf)
  :_Length(0)
  ,_Data(NULL)
{
  *this = _aBuf.c_str();
}