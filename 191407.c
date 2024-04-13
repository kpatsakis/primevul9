static inline void RemoveFreeBlock(void *block,const size_t i)
{
  void
    *next,
    *previous;

  next=NextBlockInList(block);
  previous=PreviousBlockInList(block);
  if (previous == (void *) NULL)
    memory_pool.blocks[i]=next;
  else
    NextBlockInList(previous)=next;
  if (next != (void *) NULL)
    PreviousBlockInList(next)=previous;
}