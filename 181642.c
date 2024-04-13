bool KaxBlockGroup::GetBlockDuration(uint64 &TheTimecode) const
{
  KaxBlockDuration * myDuration = static_cast<KaxBlockDuration *>(FindElt(EBML_INFO(KaxBlockDuration)));
  if (myDuration == NULL) {
    return false;
  }

  assert(ParentTrack != NULL);
  TheTimecode = uint64(*myDuration) * ParentTrack->GlobalTimecodeScale();
  return true;
}