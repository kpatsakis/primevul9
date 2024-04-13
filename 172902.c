cmyk_to_rgb(JSAMPLE c, JSAMPLE m, JSAMPLE y, JSAMPLE k, JSAMPLE *r, JSAMPLE *g,
            JSAMPLE *b)
{
  *r = (JSAMPLE)((double)c * (double)k / 255.0 + 0.5);
  *g = (JSAMPLE)((double)m * (double)k / 255.0 + 0.5);
  *b = (JSAMPLE)((double)y * (double)k / 255.0 + 0.5);
}