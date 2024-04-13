static int SceneCompare(const void *x,const void *y)
{
  const Image
    **image_1,
    **image_2;

  image_1=(const Image **) x;
  image_2=(const Image **) y;
  return((int) ((*image_1)->scene-(*image_2)->scene));
}