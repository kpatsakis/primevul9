uint64 KaxInternalBlock::ClusterPosition() const
{
  assert(ParentCluster != NULL); // impossible otherwise
  return ParentCluster->GetPosition();
}