static MagickBooleanType ExpandHeap(size_t size)
{
  DataSegmentInfo
    *segment_info;

  MagickBooleanType
    mapped;

  ssize_t
    i;

  void
    *block;

  size_t
    blocksize;

  void
    *segment;

  blocksize=((size+12*sizeof(size_t))+SegmentSize-1) & -SegmentSize;
  assert(memory_pool.number_segments < MaxSegments);
  segment=MapBlob(-1,IOMode,0,blocksize);
  mapped=segment != (void *) NULL ? MagickTrue : MagickFalse;
  if (segment == (void *) NULL)
    segment=(void *) memory_methods.acquire_memory_handler(blocksize);
  if (segment == (void *) NULL)
    return(MagickFalse);
  segment_info=(DataSegmentInfo *) free_segments;
  free_segments=segment_info->next;
  segment_info->mapped=mapped;
  segment_info->length=blocksize;
  segment_info->allocation=segment;
  segment_info->bound=(char *) segment+blocksize;
  i=(ssize_t) memory_pool.number_segments-1;
  for ( ; (i >= 0) && (memory_pool.segments[i]->allocation > segment); i--)
    memory_pool.segments[i+1]=memory_pool.segments[i];
  memory_pool.segments[i+1]=segment_info;
  memory_pool.number_segments++;
  size=blocksize-12*sizeof(size_t);
  block=(char *) segment_info->allocation+4*sizeof(size_t);
  *BlockHeader(block)=size | PreviousBlockBit;
  *BlockFooter(block,size)=size;
  InsertFreeBlock(block,AllocationPolicy(size));
  block=NextBlock(block);
  assert(block < segment_info->bound);
  *BlockHeader(block)=2*sizeof(size_t);
  *BlockHeader(NextBlock(block))=PreviousBlockBit;
  return(MagickTrue);
}