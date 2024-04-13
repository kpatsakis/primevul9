static void *AcquireBlock(size_t size)
{
  size_t
    i;

  void
    *block;

  /*
    Find free block.
  */
  size=(size_t) (size+sizeof(size_t)+6*sizeof(size_t)-1) & -(4U*sizeof(size_t));
  i=AllocationPolicy(size);
  block=memory_pool.blocks[i];
  while ((block != (void *) NULL) && (SizeOfBlock(block) < size))
    block=NextBlockInList(block);
  if (block == (void *) NULL)
    {
      i++;
      while (memory_pool.blocks[i] == (void *) NULL)
        i++;
      block=memory_pool.blocks[i];
      if (i >= MaxBlocks)
        return((void *) NULL);
    }
  assert((*BlockHeader(NextBlock(block)) & PreviousBlockBit) == 0);
  assert(SizeOfBlock(block) >= size);
  RemoveFreeBlock(block,AllocationPolicy(SizeOfBlock(block)));
  if (SizeOfBlock(block) > size)
    {
      size_t
        blocksize;

      void
        *next;

      /*
        Split block.
      */
      next=(char *) block+size;
      blocksize=SizeOfBlock(block)-size;
      *BlockHeader(next)=blocksize;
      *BlockFooter(next,blocksize)=blocksize;
      InsertFreeBlock(next,AllocationPolicy(blocksize));
      *BlockHeader(block)=size | (*BlockHeader(block) & ~SizeMask);
    }
  assert(size == SizeOfBlock(block));
  *BlockHeader(NextBlock(block))|=PreviousBlockBit;
  memory_pool.allocation+=size;
  return(block);
}