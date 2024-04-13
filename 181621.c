uint64 KaxInternalBlock::ReadInternalHead(IOCallback & input)
{
  binary Buffer[5], *cursor = Buffer;
  uint64 Result = input.read(cursor, 4);
  if (Result != 4)
    return Result;

  // update internal values
  TrackNumber = *cursor++;
  if ((TrackNumber & 0x80) == 0) {
    // there is extra data
    if ((TrackNumber & 0x40) == 0) {
      // We don't support track numbers that large !
      return Result;
    }
    Result += input.read(&Buffer[4], 1);
    TrackNumber = (TrackNumber & 0x3F) << 8;
    TrackNumber += *cursor++;
  } else {
    TrackNumber &= 0x7F;
  }


  big_int16 b16;
  b16.Eval(cursor);
  assert(ParentCluster != NULL);
  Timecode = ParentCluster->GetBlockGlobalTimecode(int16(b16));
  bLocalTimecodeUsed = false;
  cursor += 2;

  return Result;
}