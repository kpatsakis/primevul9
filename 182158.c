static int IntensityCompare(const void *x,const void *y)
{
  DiversityPacket
    *color_1,
    *color_2;

  int
    diversity;

  color_1=(DiversityPacket *) x;
  color_2=(DiversityPacket *) y;
  diversity=(int) (DiversityPixelIntensity(color_2)-
    DiversityPixelIntensity(color_1));
  return(diversity);
}