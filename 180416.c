msg_fifo_free (struct msg_fifo *fifo)
{
  msg_fifo_flush (fifo);

  XFREE (MTYPE_OSPF_API_FIFO, fifo);
}