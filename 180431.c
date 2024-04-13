msg_fifo_pop (struct msg_fifo *fifo)
{
  struct msg *msg;

  msg = fifo->head;
  if (msg)
    {
      fifo->head = msg->next;

      if (fifo->head == NULL)
	fifo->tail = NULL;

      fifo->count--;
    }
  return msg;
}