uint16 KaxBlockGroup::TrackNumber() const
{
  KaxInternalBlock & MyBlock = *static_cast<KaxBlock *>(this->FindElt(EBML_INFO(KaxBlock)));
  return MyBlock.TrackNum();
}