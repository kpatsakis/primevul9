api_opaque_lsa_print (struct lsa_header *data)
{
  struct opaque_lsa
  {
    struct lsa_header header;
    u_char mydata[];
  };

  struct opaque_lsa *olsa;
  int opaquelen;
  int i;

  ospf_lsa_header_dump (data);

  olsa = (struct opaque_lsa *) data;

  opaquelen = ntohs (data->length) - OSPF_LSA_HEADER_SIZE;
  zlog_debug ("apiserver_lsa_print: opaquelen=%d\n", opaquelen);

  for (i = 0; i < opaquelen; i++)
    {
      zlog_debug ("0x%x ", olsa->mydata[i]);
    }
  zlog_debug ("\n");
}