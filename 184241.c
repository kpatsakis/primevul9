unset_addr_list_add(UnsetAddrList* list, int offset, struct _Node* node)
{
  UnsetAddr* p;
  int size;

  if (list->num >= list->alloc) {
    size = list->alloc * 2;
    p = (UnsetAddr* )xrealloc(list->us, sizeof(UnsetAddr) * size);
    CHECK_NULL_RETURN_MEMERR(p);
    list->alloc = size;
    list->us    = p;
  }

  list->us[list->num].offset = offset;
  list->us[list->num].target = node;
  list->num++;
  return 0;
}