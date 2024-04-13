KaxBlockBlob::operator KaxSimpleBlock &()
{
  assert(bUseSimpleBlock);
  assert(Block.simpleblock);
  return *Block.simpleblock;
}