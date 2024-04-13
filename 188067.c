bool UTFstring::wcscmp_internal(const wchar_t *str1, const wchar_t *str2)
{
  size_t Index=0;
  while (str1[Index] == str2[Index] && str1[Index] != 0) {
    Index++;
  }
  return (str1[Index] == str2[Index]);
}