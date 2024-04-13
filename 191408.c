static inline size_t AllocationPolicy(size_t size)
{
  size_t
    blocksize;

  /*
    The linear distribution.
  */
  assert(size != 0);
  assert(size % (4*sizeof(size_t)) == 0);
  if (size <= BlockThreshold)
    return(size/(4*sizeof(size_t)));
  /*
    Check for the largest block size.
  */
  if (size > (size_t) (BlockThreshold*(1L << (MaxBlockExponent-1L))))
    return(MaxBlocks-1L);
  /*
    Otherwise use a power of two distribution.
  */
  blocksize=BlockThreshold/(4*sizeof(size_t));
  for ( ; size > BlockThreshold; size/=2)
    blocksize++;
  assert(blocksize > (BlockThreshold/(4*sizeof(size_t))));
  assert(blocksize < (MaxBlocks-1L));
  return(blocksize);
}