filepos_t KaxInternalBlock::RenderData(IOCallback & output, bool /* bForceRender */, bool /* bSaveDefault */)
{
  if (myBuffers.size() == 0) {
    return 0;
  } else {
    assert(TrackNumber < 0x4000);
    binary BlockHead[5], *cursor = BlockHead;
    unsigned int i;

    if (myBuffers.size() == 1) {
      SetSize_(4);
      mLacing = LACING_NONE;
    } else {
      if (mLacing == LACING_NONE)
        mLacing = LACING_EBML; // supposedly the best of all
      SetSize_(4 + 1); // 1 for the lacing head (number of laced elements)
    }
    if (TrackNumber > 0x80)
      SetSize_(GetSize() + 1);

    // write Block Head
    if (TrackNumber < 0x80) {
      *cursor++ = TrackNumber | 0x80; // set the first bit to 1
    } else {
      *cursor++ = (TrackNumber >> 8) | 0x40; // set the second bit to 1
      *cursor++ = TrackNumber & 0xFF;
    }

    assert(ParentCluster != NULL);
    int16 ActualTimecode = ParentCluster->GetBlockLocalTimecode(Timecode);
    big_int16 b16(ActualTimecode);
    b16.Fill(cursor);
    cursor += 2;

    *cursor = 0; // flags

    if (mLacing == LACING_AUTO) {
      mLacing = GetBestLacingType();
    }

    // invisible flag
    if (mInvisible)
      *cursor = 0x08;

    if (bIsSimple) {
      if (bIsKeyframe)
        *cursor |= 0x80;
      if (bIsDiscardable)
        *cursor |= 0x01;
    }

    // lacing flag
    switch (mLacing) {
      case LACING_XIPH:
        *cursor++ |= 0x02;
        break;
      case LACING_EBML:
        *cursor++ |= 0x06;
        break;
      case LACING_FIXED:
        *cursor++ |= 0x04;
        break;
      case LACING_NONE:
        break;
      default:
        assert(0);
    }

    output.writeFully(BlockHead, 4 + ((TrackNumber > 0x80) ? 1 : 0));

    binary tmpValue;
    switch (mLacing) {
      case LACING_XIPH:
        // number of laces
        tmpValue = myBuffers.size()-1;
        output.writeFully(&tmpValue, 1);

        // set the size of each member in the lace
        for (i=0; i<myBuffers.size()-1; i++) {
          tmpValue = 0xFF;
          uint16 tmpSize = myBuffers[i]->Size();
          while (tmpSize >= 0xFF) {
            output.writeFully(&tmpValue, 1);
            SetSize_(GetSize() + 1);
            tmpSize -= 0xFF;
          }
          tmpValue = binary(tmpSize);
          output.writeFully(&tmpValue, 1);
          SetSize_(GetSize() + 1);
        }
        break;
      case LACING_EBML:
        // number of laces
        tmpValue = myBuffers.size()-1;
        output.writeFully(&tmpValue, 1);
        {
          int64 _Size;
          int _CodedSize;
          binary _FinalHead[8]; // 64 bits max coded size

          _Size = myBuffers[0]->Size();

          _CodedSize = CodedSizeLength(_Size, 0, IsFiniteSize());

          // first size in the lace is not a signed
          CodedValueLength(_Size, _CodedSize, _FinalHead);
          output.writeFully(_FinalHead, _CodedSize);
          SetSize_(GetSize() + _CodedSize);

          // set the size of each member in the lace
          for (i=1; i<myBuffers.size()-1; i++) {
            _Size = int64(myBuffers[i]->Size()) - int64(myBuffers[i-1]->Size());
            _CodedSize = CodedSizeLengthSigned(_Size, 0);
            CodedValueLengthSigned(_Size, _CodedSize, _FinalHead);
            output.writeFully(_FinalHead, _CodedSize);
            SetSize_(GetSize() + _CodedSize);
          }
        }
        break;
      case LACING_FIXED:
        // number of laces
        tmpValue = myBuffers.size()-1;
        output.writeFully(&tmpValue, 1);
        break;
      case LACING_NONE:
        break;
      default:
        assert(0);
    }

    // put the data of each frame
    for (i=0; i<myBuffers.size(); i++) {
      output.writeFully(myBuffers[i]->Buffer(), myBuffers[i]->Size());
      SetSize_(GetSize() + myBuffers[i]->Size());
    }
  }

  return GetSize();
}