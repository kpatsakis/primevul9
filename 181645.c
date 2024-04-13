bool KaxBlockGroup::AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, const KaxBlockGroup & PastBlock, LacingType lacing)
{
  //  assert(past_timecode < 0);

  KaxBlock & theBlock = GetChild<KaxBlock>(*this);
  assert(ParentCluster != NULL);
  theBlock.SetParent(*ParentCluster);
  ParentTrack = &track;
  bool bRes = theBlock.AddFrame(track, timecode, buffer, lacing);

  KaxReferenceBlock & thePastRef = GetChild<KaxReferenceBlock>(*this);
  thePastRef.SetReferencedBlock(PastBlock);
  thePastRef.SetParentBlock(*this);

  return bRes;
}