decode_rd_as (u_char *pnt, struct rd_as *rd_as)
{
  rd_as->as = (u_int16_t) *pnt++ << 8;
  rd_as->as |= (u_int16_t) *pnt++;
  
  rd_as->val = ((u_int32_t) *pnt++ << 24);
  rd_as->val |= ((u_int32_t) *pnt++ << 16);
  rd_as->val |= ((u_int32_t) *pnt++ << 8);
  rd_as->val |= (u_int32_t) *pnt;
}