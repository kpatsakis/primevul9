decode_label (u_char *pnt)
{
  u_int32_t l;

  l = ((u_int32_t) *pnt++ << 12);
  l |= (u_int32_t) *pnt++ << 4;
  l |= (u_int32_t) ((*pnt & 0xf0) >> 4);
  return l;
}