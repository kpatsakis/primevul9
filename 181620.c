uint64 KaxBlockGroup::GlobalTimecode() const
{
  assert(ParentCluster != NULL); // impossible otherwise
  KaxInternalBlock & MyBlock = *static_cast<KaxBlock *>(this->FindElt(EBML_INFO(KaxBlock)));
  return MyBlock.GlobalTimecode();

}