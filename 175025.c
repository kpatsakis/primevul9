static std::string to_fourcc(uint32_t code)
{
  std::string str("    ");
  str[0] = static_cast<char>((code>>24) & 0xFF);
  str[1] = static_cast<char>((code>>16) & 0xFF);
  str[2] = static_cast<char>((code>> 8) & 0xFF);
  str[3] = static_cast<char>((code>> 0) & 0xFF);

  return str;
}