UTFstring & UTFstring::operator=(const UTFstring & _aBuf)
{
  *this = _aBuf.c_str();
  return *this;
}