host_find_interfaces(void)
{
ip_address_item *running_interfaces = NULL;

if (!local_interface_data)
  {
  void *reset_item = store_mark();
  ip_address_item *dlist = host_build_ifacelist(CUS local_interfaces,
    US"local_interfaces");
  ip_address_item *xlist = host_build_ifacelist(CUS extra_local_interfaces,
    US"extra_local_interfaces");
  ip_address_item *ipa;

  if (!dlist) dlist = xlist;
  else
    {
    for (ipa = dlist; ipa->next; ipa = ipa->next) ;
    ipa->next = xlist;
    }

  for (ipa = dlist; ipa; ipa = ipa->next)
    {
    if (Ustrcmp(ipa->address, "0.0.0.0") == 0 ||
        Ustrcmp(ipa->address, "::0") == 0)
      {
      BOOL ipv6 = ipa->address[0] == ':';
      if (!running_interfaces)
        running_interfaces = os_find_running_interfaces();
      for (ip_address_item * ipa2 = running_interfaces; ipa2; ipa2 = ipa2->next)
        if ((Ustrchr(ipa2->address, ':') != NULL) == ipv6)
          local_interface_data = add_unique_interface(local_interface_data,
						      ipa2);
      }
    else
      {
      local_interface_data = add_unique_interface(local_interface_data, ipa);
      DEBUG(D_interface)
        {
        debug_printf("Configured local interface: address=%s", ipa->address);
        if (ipa->port != 0) debug_printf(" port=%d", ipa->port);
        debug_printf("\n");
        }
      }
    }
  store_reset(reset_item);
  }

return local_interface_data;
}