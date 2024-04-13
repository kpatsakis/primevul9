bool UTFstring::operator==(const UTFstring& _aStr) const
{
  if ((_Data == NULL) && (_aStr._Data == NULL))
    return true;
  if ((_Data == NULL) || (_aStr._Data == NULL))
    return false;
  return wcscmp_internal(_Data, _aStr._Data);
}