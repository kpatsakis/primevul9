bool KaxBlockBlob::AddFrameAuto(const KaxTrackEntry & track, uint64 timecode, DataBuffer & buffer, LacingType lacing, const KaxBlockBlob * PastBlock, const KaxBlockBlob * ForwBlock)
{
  bool bResult = false;
#if MATROSKA_VERSION >= 2
  if ((SimpleBlockMode == BLOCK_BLOB_ALWAYS_SIMPLE) || (SimpleBlockMode == BLOCK_BLOB_SIMPLE_AUTO && PastBlock == NULL && ForwBlock == NULL)) {
    assert(bUseSimpleBlock == true);
    if (Block.simpleblock == NULL) {
      Block.simpleblock = new KaxSimpleBlock();
      Block.simpleblock->SetParent(*ParentCluster);
    }

    bResult = Block.simpleblock->AddFrame(track, timecode, buffer, lacing);
    if (PastBlock == NULL && ForwBlock == NULL) {
      Block.simpleblock->SetKeyframe(true);
      Block.simpleblock->SetDiscardable(false);
    } else {
      Block.simpleblock->SetKeyframe(false);
      if ((ForwBlock == NULL || ((const KaxInternalBlock &)*ForwBlock).GlobalTimecode() <= timecode) &&
          (PastBlock == NULL || ((const KaxInternalBlock &)*PastBlock).GlobalTimecode() <= timecode))
        Block.simpleblock->SetDiscardable(false);
      else
        Block.simpleblock->SetDiscardable(true);
    }
  }
  else
#endif
    if (ReplaceSimpleByGroup())
      bResult = Block.group->AddFrame(track, timecode, buffer, PastBlock, ForwBlock, lacing);

  return bResult;
}