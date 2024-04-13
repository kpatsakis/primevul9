unset_addr_list_end(UnsetAddrList* list)
{
  if (IS_NOT_NULL(list->us))
    xfree(list->us);
}