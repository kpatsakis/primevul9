static int PopularityCompare(const void *x,const void *y)
{
  DiversityPacket
    *color_1,
    *color_2;

  color_1=(DiversityPacket *) x;
  color_2=(DiversityPacket *) y;
  return((int) color_2->count-(int) color_1->count);
}