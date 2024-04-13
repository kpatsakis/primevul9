uint64 KaxBlockGroup::ClusterPosition() const
{
  assert(ParentCluster != NULL); // impossible otherwise
  return ParentCluster->GetPosition();
}