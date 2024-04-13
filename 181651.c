void KaxBlockBlob::SetBlockGroup( KaxBlockGroup &BlockRef )
{
  assert(!bUseSimpleBlock);
  Block.group = &BlockRef;
}