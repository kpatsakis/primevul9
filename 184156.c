unset_addr_list_init(UnsetAddrList* list, int size)
{
  UnsetAddr* p = (UnsetAddr* )xmalloc(sizeof(UnsetAddr)* size);
  CHECK_NULL_RETURN_MEMERR(p);

  list->num   = 0;
  list->alloc = size;
  list->us    = p;
  return 0;
}