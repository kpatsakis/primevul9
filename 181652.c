bool KaxBlockGroup::AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, const KaxBlockGroup & PastBlock, const KaxBlockGroup & ForwBlock, LacingType lacing)
{
  //  assert(past_timecode < 0);

  //  assert(forw_timecode > 0);

  KaxBlock & theBlock = GetChild<KaxBlock>(*this);
  assert(ParentCluster != NULL);
  theBlock.SetParent(*ParentCluster);
  ParentTrack = &track;
  bool bRes = theBlock.AddFrame(track, timecode, buffer, lacing);

  KaxReferenceBlock & thePastRef = GetChild<KaxReferenceBlock>(*this);
  thePastRef.SetReferencedBlock(PastBlock);
  thePastRef.SetParentBlock(*this);

  KaxReferenceBlock & theFutureRef = AddNewChild<KaxReferenceBlock>(*this);
  theFutureRef.SetReferencedBlock(ForwBlock);
  theFutureRef.SetParentBlock(*this);

  return bRes;
}