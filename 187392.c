static void PruneChild(CubeInfo *cube_info,const NodeInfo *node_info)
{
  NodeInfo
    *parent;

  register ssize_t
    i;

  size_t
    number_children;

  /*
    Traverse any children.
  */
  number_children=cube_info->associate_alpha == MagickFalse ? 8UL : 16UL;
  for (i=0; i < (ssize_t) number_children; i++)
    if (node_info->child[i] != (NodeInfo *) NULL)
      PruneChild(cube_info,node_info->child[i]);
  /*
    Merge color statistics into parent.
  */
  parent=node_info->parent;
  parent->number_unique+=node_info->number_unique;
  parent->total_color.red+=node_info->total_color.red;
  parent->total_color.green+=node_info->total_color.green;
  parent->total_color.blue+=node_info->total_color.blue;
  parent->total_color.opacity+=node_info->total_color.opacity;
  parent->child[node_info->id]=(NodeInfo *) NULL;
  cube_info->nodes--;
}