filepos_t EbmlUnicodeString::ReadData(IOCallback & input, ScopeMode ReadFully)
{
  if (ReadFully != SCOPE_NO_DATA) {
    if (GetSize() == 0) {
      Value = UTFstring::value_type(0);
      SetValueIsSet();
    } else {
      char *Buffer = new (std::nothrow) char[GetSize()+1];
      if (Buffer == NULL) {
        // impossible to read, skip it
        input.setFilePointer(GetSize(), seek_current);
      } else {
        input.readFully(Buffer, GetSize());
        if (Buffer[GetSize()-1] != 0) {
          Buffer[GetSize()] = 0;
        }

        Value.SetUTF8(Buffer); // implicit conversion to std::string
        delete [] Buffer;
        SetValueIsSet();
      }
    }
  }

  return GetSize();
}