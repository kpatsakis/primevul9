KaxBlockGroup::operator KaxInternalBlock &() {
  KaxBlock & theBlock = GetChild<KaxBlock>(*this);
  return theBlock;
}