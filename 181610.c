void KaxBlockGroup::SetBlockDuration(uint64 TimeLength)
{
  assert(ParentTrack != NULL);
  int64 scale = ParentTrack->GlobalTimecodeScale();
  KaxBlockDuration & myDuration = *static_cast<KaxBlockDuration *>(FindFirstElt(EBML_INFO(KaxBlockDuration), true));
  *(static_cast<EbmlUInteger *>(&myDuration)) = TimeLength / uint64(scale);
}