KaxBlockBlob::operator KaxBlockGroup &()
{
  assert(!bUseSimpleBlock);
  assert(Block.group);
  return *Block.group;
}