void KaxInternalBlock::SetParent(KaxCluster & aParentCluster)
{
  ParentCluster = &aParentCluster;
  if (bLocalTimecodeUsed) {
    Timecode = aParentCluster.GetBlockGlobalTimecode(LocalTimecode);
    bLocalTimecodeUsed = false;
  }
}