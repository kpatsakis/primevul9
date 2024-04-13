static size_t QuantizeErrorFlatten(const CubeInfo *cube_info,
  const NodeInfo *node_info,const ssize_t offset,
  MagickRealType *quantize_error)
{
  register ssize_t
    i;

  size_t
    n,
    number_children;

  if (offset >= (ssize_t) cube_info->nodes)
    return(0);
  quantize_error[offset]=node_info->quantize_error;
  n=1;
  number_children=cube_info->associate_alpha == MagickFalse ? 8UL : 16UL;
  for (i=0; i < (ssize_t) number_children ; i++)
    if (node_info->child[i] != (NodeInfo *) NULL)
      n+=QuantizeErrorFlatten(cube_info,node_info->child[i],offset+n,
        quantize_error);
  return(n);
}