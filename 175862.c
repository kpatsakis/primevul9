decode_rd_ip (u_char *pnt, struct rd_ip *rd_ip)
{
  memcpy (&rd_ip->ip, pnt, 4);
  pnt += 4;
  
  rd_ip->val = ((u_int16_t) *pnt++ << 8);
  rd_ip->val |= (u_int16_t) *pnt;
}