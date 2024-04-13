UTFstring::UTFstring(std::wstring const &_aBuf)
  :_Length(0)
  ,_Data(NULL)
{
  *this = _aBuf.c_str();
}