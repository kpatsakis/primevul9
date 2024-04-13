msg_fifo_flush (struct msg_fifo *fifo)
{
  struct msg *op;
  struct msg *next;

  for (op = fifo->head; op; op = next)
    {
      next = op->next;
      msg_free (op);
    }

  fifo->head = fifo->tail = NULL;
  fifo->count = 0;
}