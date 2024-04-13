filepos_t KaxBlockVirtual::UpdateSize(bool /* bSaveDefault */, bool /* bForceRender */)
{
  assert(TrackNumber < 0x4000);
  binary *cursor = EbmlBinary::GetBuffer();
  // fill data
  if (TrackNumber < 0x80) {
    assert(GetSize() >= 4);
    *cursor++ = TrackNumber | 0x80; // set the first bit to 1
  } else {
    assert(GetSize() >= 5);
    *cursor++ = (TrackNumber >> 8) | 0x40; // set the second bit to 1
    *cursor++ = TrackNumber & 0xFF;
  }

  assert(ParentCluster != NULL);
  int16 ActualTimecode = ParentCluster->GetBlockLocalTimecode(Timecode);
  big_int16 b16(ActualTimecode);
  b16.Fill(cursor);
  cursor += 2;

  *cursor++ = 0; // flags

  return GetSize();
}