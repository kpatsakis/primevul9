uint32_t from_fourcc(const char* string)
{
  return ((string[0]<<24) |
          (string[1]<<16) |
          (string[2]<< 8) |
          (string[3]));
}