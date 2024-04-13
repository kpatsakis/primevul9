filepos_t EbmlUnicodeString::RenderData(IOCallback & output, bool /* bForceRender */, bool /* bWithDefault */)
{
  uint32 Result = Value.GetUTF8().length();

  if (Result != 0) {
    output.writeFully(Value.GetUTF8().c_str(), Result);
  }

  if (Result < GetDefaultSize()) {
    // pad the rest with 0
    binary *Pad = new (std::nothrow) binary[GetDefaultSize() - Result];
    if (Pad != NULL) {
      memset(Pad, 0x00, GetDefaultSize() - Result);
      output.writeFully(Pad, GetDefaultSize() - Result);

      Result = GetDefaultSize();
      delete [] Pad;
    }
  }

  return Result;
}