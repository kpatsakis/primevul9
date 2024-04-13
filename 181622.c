filepos_t KaxInternalBlock::UpdateSize(bool /* bSaveDefault */, bool /* bForceRender */)
{
  LacingType LacingHere;
  assert(EbmlBinary::GetBuffer() == NULL); // Data is not used for KaxInternalBlock
  assert(TrackNumber < 0x4000); // no more allowed for the moment
  unsigned int i;

  // compute the final size of the data
  switch (myBuffers.size()) {
    case 0:
      SetSize_(0);
      break;
    case 1:
      SetSize_(4 + myBuffers[0]->Size());
      break;
    default:
      SetSize_(4 + 1); // 1 for the lacing head
      if (mLacing == LACING_AUTO)
        LacingHere = GetBestLacingType();
      else
        LacingHere = mLacing;
      switch (LacingHere) {
        case LACING_XIPH:
          for (i=0; i<myBuffers.size()-1; i++) {
            SetSize_(GetSize() + myBuffers[i]->Size() + (myBuffers[i]->Size() / 0xFF + 1));
          }
          break;
        case LACING_EBML:
          SetSize_(GetSize() + myBuffers[0]->Size() + CodedSizeLength(myBuffers[0]->Size(), 0, IsFiniteSize()));
          for (i=1; i<myBuffers.size()-1; i++) {
            SetSize_(GetSize() + myBuffers[i]->Size() + CodedSizeLengthSigned(int64(myBuffers[i]->Size()) - int64(myBuffers[i-1]->Size()), 0));
          }
          break;
        case LACING_FIXED:
          for (i=0; i<myBuffers.size()-1; i++) {
            SetSize_(GetSize() + myBuffers[i]->Size());
          }
          break;
        default:
          i = 0;
          assert(0);
      }
      // Size of the last frame (not in lace)
      SetSize_(GetSize() + myBuffers[i]->Size());
      break;
  }

  if (TrackNumber >= 0x80)
    SetSize_(GetSize() + 1); // the size will be coded with one more octet

  return GetSize();
}