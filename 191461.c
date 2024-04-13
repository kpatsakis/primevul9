static inline void InsertFreeBlock(void *block,const size_t i)
{
  void
    *next,
    *previous;

  size_t
    size;

  size=SizeOfBlock(block);
  previous=(void *) NULL;
  next=memory_pool.blocks[i];
  while ((next != (void *) NULL) && (SizeOfBlock(next) < size))
  {
    previous=next;
    next=NextBlockInList(next);
  }
  PreviousBlockInList(block)=previous;
  NextBlockInList(block)=next;
  if (previous != (void *) NULL)
    NextBlockInList(previous)=block;
  else
    memory_pool.blocks[i]=block;
  if (next != (void *) NULL)
    PreviousBlockInList(next)=block;
}