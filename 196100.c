merge_queue_lists(queue_filename *a, queue_filename *b)
{
queue_filename *first = NULL;
queue_filename **append = &first;

while (a && b)
  {
  int d;
  if ((d = Ustrncmp(a->text, b->text, 6)) == 0)
    d = Ustrcmp(a->text + 14, b->text + 14);
  if (d < 0)
    {
    *append = a;
    append= &a->next;
    a = a->next;
    }
  else
    {
    *append = b;
    append= &b->next;
    b = b->next;
    }
  }

*append = a ? a : b;
return first;
}