bool KaxBlockGroup::AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, LacingType lacing)
{
  KaxBlock & theBlock = GetChild<KaxBlock>(*this);
  assert(ParentCluster != NULL);
  theBlock.SetParent(*ParentCluster);
  ParentTrack = &track;
  return theBlock.AddFrame(track, timecode, buffer, lacing);
}