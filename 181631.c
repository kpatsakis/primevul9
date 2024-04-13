void KaxBlockGroup::ReleaseFrames()
{
  KaxInternalBlock & MyBlock = *static_cast<KaxBlock *>(this->FindElt(EBML_INFO(KaxBlock)));
  MyBlock.ReleaseFrames();
}