decode_rd_type (u_char *pnt)
{
  u_int16_t v;
  
  v = ((u_int16_t) *pnt++ << 8);
  v |= (u_int16_t) *pnt;
  return v;
}