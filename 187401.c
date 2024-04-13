static CubeInfo *GetCubeInfo(const QuantizeInfo *quantize_info,
  const size_t depth,const size_t maximum_colors)
{
  CubeInfo
    *cube_info;

  MagickRealType
    sum,
    weight;

  register ssize_t
    i;

  size_t
    length;

  /*
    Initialize tree to describe color cube_info.
  */
  cube_info=(CubeInfo *) AcquireMagickMemory(sizeof(*cube_info));
  if (cube_info == (CubeInfo *) NULL)
    return((CubeInfo *) NULL);
  (void) memset(cube_info,0,sizeof(*cube_info));
  cube_info->depth=depth;
  if (cube_info->depth > MaxTreeDepth)
    cube_info->depth=MaxTreeDepth;
  if (cube_info->depth < 2)
    cube_info->depth=2;
  cube_info->maximum_colors=maximum_colors;
  /*
    Initialize root node.
  */
  cube_info->root=GetNodeInfo(cube_info,0,0,(NodeInfo *) NULL);
  if (cube_info->root == (NodeInfo *) NULL)
    return((CubeInfo *) NULL);
  cube_info->root->parent=cube_info->root;
  cube_info->quantize_info=CloneQuantizeInfo(quantize_info);
  if (cube_info->quantize_info->dither == MagickFalse)
    return(cube_info);
  /*
    Initialize dither resources.
  */
  length=(size_t) (1UL << (4*(8-CacheShift)));
  cube_info->memory_info=AcquireVirtualMemory(length,sizeof(*cube_info->cache));
  if (cube_info->memory_info == (MemoryInfo *) NULL)
    return((CubeInfo *) NULL);
  cube_info->cache=(ssize_t *) GetVirtualMemoryBlob(cube_info->memory_info);
  /*
    Initialize color cache.
  */
  (void) memset(cube_info->cache,(-1),sizeof(*cube_info->cache)*length);
  /*
    Distribute weights along a curve of exponential decay.
  */
  weight=1.0;
  for (i=0; i < ErrorQueueLength; i++)
  {
    cube_info->weights[ErrorQueueLength-i-1]=PerceptibleReciprocal(weight);
    weight*=exp(log(((double) QuantumRange+1.0))/(ErrorQueueLength-1.0));
  }
  /*
    Normalize the weighting factors.
  */
  weight=0.0;
  for (i=0; i < ErrorQueueLength; i++)
    weight+=cube_info->weights[i];
  sum=0.0;
  for (i=0; i < ErrorQueueLength; i++)
  {
    cube_info->weights[i]/=weight;
    sum+=cube_info->weights[i];
  }
  cube_info->weights[0]+=1.0-sum;
  return(cube_info);
}