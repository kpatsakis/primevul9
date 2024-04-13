static inline double MeshInterpolate(const PointInfo *delta,const double p,
  const double x,const double y)
{
  return(delta->x*x+delta->y*y+(1.0-delta->x-delta->y)*p);
}