rgb_to_cmyk(JSAMPLE r, JSAMPLE g, JSAMPLE b, JSAMPLE *c, JSAMPLE *m,
            JSAMPLE *y, JSAMPLE *k)
{
  double ctmp = 1.0 - ((double)r / 255.0);
  double mtmp = 1.0 - ((double)g / 255.0);
  double ytmp = 1.0 - ((double)b / 255.0);
  double ktmp = min(min(ctmp, mtmp), ytmp);

  if (ktmp == 1.0) ctmp = mtmp = ytmp = 0.0;
  else {
    ctmp = (ctmp - ktmp) / (1.0 - ktmp);
    mtmp = (mtmp - ktmp) / (1.0 - ktmp);
    ytmp = (ytmp - ktmp) / (1.0 - ktmp);
  }
  *c = (JSAMPLE)(255.0 - ctmp * 255.0 + 0.5);
  *m = (JSAMPLE)(255.0 - mtmp * 255.0 + 0.5);
  *y = (JSAMPLE)(255.0 - ytmp * 255.0 + 0.5);
  *k = (JSAMPLE)(255.0 - ktmp * 255.0 + 0.5);
}