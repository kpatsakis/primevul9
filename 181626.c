void KaxBlockGroup::SetParent(KaxCluster & aParentCluster) {
  ParentCluster = &aParentCluster;
  KaxBlock & theBlock = GetChild<KaxBlock>(*this);
  theBlock.SetParent( aParentCluster );
}