static inline void ConvertLuvToXYZ(const double L,const double u,const double v,
  double *X,double *Y,double *Z)
{
  double
    gamma;

  assert(X != (double *) NULL);
  assert(Y != (double *) NULL);
  assert(Z != (double *) NULL);
  if (L > (CIEK*CIEEpsilon))
    *Y=(double) pow((L+16.0)/116.0,3.0);
  else
    *Y=L/CIEK;
  gamma=PerceptibleReciprocal((((52.0*L*PerceptibleReciprocal(u+13.0*L* 
    (4.0*D65X/(D65X+15.0*D65Y+3.0*D65Z))))-1.0)/3.0)-(-1.0/3.0));
  *X=gamma*((*Y*((39.0*L*PerceptibleReciprocal(v+13.0*L*(9.0*D65Y/
    (D65X+15.0*D65Y+3.0*D65Z))))-5.0))+5.0*(*Y));
  *Z=(*X*(((52.0*L*PerceptibleReciprocal(u+13.0*L*(4.0*D65X/
    (D65X+15.0*D65Y+3.0*D65Z))))-1.0)/3.0))-5.0*(*Y);
}