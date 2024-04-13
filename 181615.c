KaxBlockBlob::operator const KaxBlockGroup &() const
{
  assert(!bUseSimpleBlock);
  assert(Block.group);
  return *Block.group;
}