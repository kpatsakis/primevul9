LacingType KaxInternalBlock::GetBestLacingType() const {
  int XiphLacingSize, EbmlLacingSize, i;
  bool SameSize = true;

  if (myBuffers.size() <= 1)
    return LACING_NONE;

  XiphLacingSize = 1; // Number of laces is stored in 1 byte.
  EbmlLacingSize = 1;
  for (i = 0; i < (int)myBuffers.size() - 1; i++) {
    if (myBuffers[i]->Size() != myBuffers[i + 1]->Size())
      SameSize = false;
    XiphLacingSize += myBuffers[i]->Size() / 255 + 1;
  }
  EbmlLacingSize += CodedSizeLength(myBuffers[0]->Size(), 0, IsFiniteSize());
  for (i = 1; i < (int)myBuffers.size() - 1; i++)
    EbmlLacingSize += CodedSizeLengthSigned(int64(myBuffers[i]->Size()) - int64(myBuffers[i - 1]->Size()), 0);
  if (SameSize)
    return LACING_FIXED;
  else if (XiphLacingSize < EbmlLacingSize)
    return LACING_XIPH;
  else
    return LACING_EBML;
}