unsigned int KaxBlockGroup::ReferenceCount() const
{
  unsigned int Result = 0;
  KaxReferenceBlock * MyBlockAdds = static_cast<KaxReferenceBlock *>(FindFirstElt(EBML_INFO(KaxReferenceBlock)));
  if (MyBlockAdds != NULL) {
    Result++;
    while ((MyBlockAdds = static_cast<KaxReferenceBlock *>(FindNextElt(*MyBlockAdds))) != NULL) {
      Result++;
    }
  }
  return Result;
}