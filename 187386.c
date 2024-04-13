static void Riemersma(Image *image,CacheView *image_view,CubeInfo *cube_info,
  const size_t level,const unsigned int direction)
{
  if (level == 1)
    switch (direction)
    {
      case WestGravity:
      {
        (void) RiemersmaDither(image,image_view,cube_info,EastGravity);
        (void) RiemersmaDither(image,image_view,cube_info,SouthGravity);
        (void) RiemersmaDither(image,image_view,cube_info,WestGravity);
        break;
      }
      case EastGravity:
      {
        (void) RiemersmaDither(image,image_view,cube_info,WestGravity);
        (void) RiemersmaDither(image,image_view,cube_info,NorthGravity);
        (void) RiemersmaDither(image,image_view,cube_info,EastGravity);
        break;
      }
      case NorthGravity:
      {
        (void) RiemersmaDither(image,image_view,cube_info,SouthGravity);
        (void) RiemersmaDither(image,image_view,cube_info,EastGravity);
        (void) RiemersmaDither(image,image_view,cube_info,NorthGravity);
        break;
      }
      case SouthGravity:
      {
        (void) RiemersmaDither(image,image_view,cube_info,NorthGravity);
        (void) RiemersmaDither(image,image_view,cube_info,WestGravity);
        (void) RiemersmaDither(image,image_view,cube_info,SouthGravity);
        break;
      }
      default:
        break;
    }
  else
    switch (direction)
    {
      case WestGravity:
      {
        Riemersma(image,image_view,cube_info,level-1,NorthGravity);
        (void) RiemersmaDither(image,image_view,cube_info,EastGravity);
        Riemersma(image,image_view,cube_info,level-1,WestGravity);
        (void) RiemersmaDither(image,image_view,cube_info,SouthGravity);
        Riemersma(image,image_view,cube_info,level-1,WestGravity);
        (void) RiemersmaDither(image,image_view,cube_info,WestGravity);
        Riemersma(image,image_view,cube_info,level-1,SouthGravity);
        break;
      }
      case EastGravity:
      {
        Riemersma(image,image_view,cube_info,level-1,SouthGravity);
        (void) RiemersmaDither(image,image_view,cube_info,WestGravity);
        Riemersma(image,image_view,cube_info,level-1,EastGravity);
        (void) RiemersmaDither(image,image_view,cube_info,NorthGravity);
        Riemersma(image,image_view,cube_info,level-1,EastGravity);
        (void) RiemersmaDither(image,image_view,cube_info,EastGravity);
        Riemersma(image,image_view,cube_info,level-1,NorthGravity);
        break;
      }
      case NorthGravity:
      {
        Riemersma(image,image_view,cube_info,level-1,WestGravity);
        (void) RiemersmaDither(image,image_view,cube_info,SouthGravity);
        Riemersma(image,image_view,cube_info,level-1,NorthGravity);
        (void) RiemersmaDither(image,image_view,cube_info,EastGravity);
        Riemersma(image,image_view,cube_info,level-1,NorthGravity);
        (void) RiemersmaDither(image,image_view,cube_info,NorthGravity);
        Riemersma(image,image_view,cube_info,level-1,EastGravity);
        break;
      }
      case SouthGravity:
      {
        Riemersma(image,image_view,cube_info,level-1,EastGravity);
        (void) RiemersmaDither(image,image_view,cube_info,NorthGravity);
        Riemersma(image,image_view,cube_info,level-1,SouthGravity);
        (void) RiemersmaDither(image,image_view,cube_info,WestGravity);
        Riemersma(image,image_view,cube_info,level-1,SouthGravity);
        (void) RiemersmaDither(image,image_view,cube_info,SouthGravity);
        Riemersma(image,image_view,cube_info,level-1,WestGravity);
        break;
      }
      default:
        break;
    }
}