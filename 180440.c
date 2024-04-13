msg_fifo_push (struct msg_fifo *fifo, struct msg *msg)
{
  if (fifo->tail)
    fifo->tail->next = msg;
  else
    fifo->head = msg;

  fifo->tail = msg;
  fifo->count++;
}