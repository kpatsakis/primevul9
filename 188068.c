void UTFstring::SetUTF8(const std::string & _aStr)
{
  UTF8string = _aStr;
  UpdateFromUTF8();
}