static size_t DefineImageColormap(Image *image,CubeInfo *cube_info,
  NodeInfo *node_info)
{
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
      (void) DefineImageColormap(image,cube_info,node_info->child[i]);
  if (node_info->number_unique != 0)
    {
      register MagickRealType
        alpha;

      register PixelPacket
        *magick_restrict q;

      /*
        Colormap entry is defined by the mean color in this cube.
      */
      q=image->colormap+image->colors;
      alpha=(MagickRealType) ((MagickOffsetType) node_info->number_unique);
      alpha=PerceptibleReciprocal(alpha);
      if (cube_info->associate_alpha == MagickFalse)
        {
          SetPixelRed(q,ClampToQuantum((MagickRealType) (alpha*
            QuantumRange*node_info->total_color.red)));
          SetPixelGreen(q,ClampToQuantum((MagickRealType) (alpha*
            QuantumRange*node_info->total_color.green)));
          SetPixelBlue(q,ClampToQuantum((MagickRealType) (alpha*
            QuantumRange*node_info->total_color.blue)));
          SetPixelOpacity(q,OpaqueOpacity);
        }
      else
        {
          MagickRealType
            opacity;

          opacity=(MagickRealType) (alpha*QuantumRange*
            node_info->total_color.opacity);
          SetPixelOpacity(q,ClampToQuantum(opacity));
          if (q->opacity == OpaqueOpacity)
            {
              SetPixelRed(q,ClampToQuantum((MagickRealType) (alpha*
                QuantumRange*node_info->total_color.red)));
              SetPixelGreen(q,ClampToQuantum((MagickRealType) (alpha*
                QuantumRange*node_info->total_color.green)));
              SetPixelBlue(q,ClampToQuantum((MagickRealType) (alpha*
                QuantumRange*node_info->total_color.blue)));
            }
          else
            {
              double
                gamma;

              gamma=(double) (QuantumScale*(QuantumRange-(double) q->opacity));
              gamma=PerceptibleReciprocal(gamma);
              SetPixelRed(q,ClampToQuantum((MagickRealType) (alpha*
                gamma*QuantumRange*node_info->total_color.red)));
              SetPixelGreen(q,ClampToQuantum((MagickRealType) (alpha*
                gamma*QuantumRange*node_info->total_color.green)));
              SetPixelBlue(q,ClampToQuantum((MagickRealType) (alpha*
                gamma*QuantumRange*node_info->total_color.blue)));
              if (node_info->number_unique > cube_info->transparent_pixels)
                {
                  cube_info->transparent_pixels=node_info->number_unique;
                  cube_info->transparent_index=(ssize_t) image->colors;
                }
            }
        }
      node_info->color_number=image->colors++;
    }
  return(image->colors);
}