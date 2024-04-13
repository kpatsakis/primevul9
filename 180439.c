msg_fifo_new ()
{
  return XCALLOC (MTYPE_OSPF_API_FIFO, sizeof (struct msg_fifo));
}