bool KaxInternalBlock::AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, LacingType lacing, bool invisible)
{
  SetValueIsSet();
  if (myBuffers.size() == 0) {
    // first frame
    Timecode = timecode;
    TrackNumber = track.TrackNumber();
    mInvisible = invisible;
    mLacing = lacing;
  }
  myBuffers.push_back(&buffer);

  // we don't allow more than 8 frames in a Block because the overhead improvement is minimal
  if (myBuffers.size() >= 8 || lacing == LACING_NONE)
    return false;

  if (lacing == LACING_XIPH)
    // decide wether a new frame can be added or not
    // a frame in a lace is not efficient when the place necessary to code it in a lace is bigger
    // than the size of a simple Block. That means more than 6 bytes (4 in struct + 2 for EBML) to code the size
    return (buffer.Size() < 6*0xFF);
  else
    return true;
}