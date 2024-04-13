void rfc822_free_address (ADDRESS **p)
{
  ADDRESS *t;

  while (*p)
  {
    t = *p;
    *p = (*p)->next;
#ifdef EXACT_ADDRESS
    FREE (&t->val);
#endif
    FREE (&t->personal);
    FREE (&t->mailbox);
    FREE (&t);
  }
}