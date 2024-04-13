bool KaxBlockGroup::AddFrame(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, const KaxBlockBlob * PastBlock, const KaxBlockBlob * ForwBlock, LacingType lacing)
{
  KaxBlock & theBlock = GetChild<KaxBlock>(*this);
  assert(ParentCluster != NULL);
  theBlock.SetParent(*ParentCluster);
  ParentTrack = &track;
  bool bRes = theBlock.AddFrame(track, timecode, buffer, lacing);

  if (PastBlock != NULL) {
    KaxReferenceBlock & thePastRef = GetChild<KaxReferenceBlock>(*this);
    thePastRef.SetReferencedBlock(PastBlock);
    thePastRef.SetParentBlock(*this);
  }

  if (ForwBlock != NULL) {
    KaxReferenceBlock & theFutureRef = AddNewChild<KaxReferenceBlock>(*this);
    theFutureRef.SetReferencedBlock(ForwBlock);
    theFutureRef.SetParentBlock(*this);
  }

  return bRes;
}