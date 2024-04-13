static void free_address (ADDRESS *a)
{
  FREE(&a->personal);
  FREE(&a->mailbox);
#ifdef EXACT_ADDRESS
  FREE(&a->val);
#endif
  FREE(&a);
}