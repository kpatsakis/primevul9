int64 KaxInternalBlock::GetDataPosition(size_t FrameNumber)
{
  int64 _Result = -1;

  if (ValueIsSet() && FrameNumber < SizeList.size()) {
    _Result = FirstFrameLocation;

    size_t _Idx = 0;
    while(FrameNumber--) {
      _Result += SizeList[_Idx++];
    }
  }

  return _Result;
}