KaxBlockBlob::operator KaxInternalBlock &()
{
  assert(Block.group);
#if MATROSKA_VERSION >= 2
  if (bUseSimpleBlock)
    return *Block.simpleblock;
  else
#endif
    return *Block.group;
}