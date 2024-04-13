prefix_rd2str (struct prefix_rd *prd, char *buf, size_t size)
{
  u_char *pnt;
  u_int16_t type;
  struct rd_as rd_as;
  struct rd_ip rd_ip;

  if (size < RD_ADDRSTRLEN)
    return NULL;

  pnt = prd->val;

  type = decode_rd_type (pnt);

  if (type == RD_TYPE_AS)
    {
      decode_rd_as (pnt + 2, &rd_as);
      snprintf (buf, size, "%u:%d", rd_as.as, rd_as.val);
      return buf;
    }
  else if (type == RD_TYPE_IP)
    {
      decode_rd_ip (pnt + 2, &rd_ip);
      snprintf (buf, size, "%s:%d", inet_ntoa (rd_ip.ip), rd_ip.val);
      return buf;
    }

  return NULL;
}