uint64 EbmlUnicodeString::UpdateSize(bool bWithDefault, bool /* bForceRender */)
{
  if (!bWithDefault && IsDefaultValue())
    return 0;

  SetSize_(Value.GetUTF8().length());
  if (GetSize() < GetDefaultSize())
    SetSize_(GetDefaultSize());

  return GetSize();
}