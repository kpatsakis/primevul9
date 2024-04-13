add_unique_interface(ip_address_item *list, ip_address_item *ipa)
{
ip_address_item *ipa2;
for (ipa2 = list; ipa2; ipa2 = ipa2->next)
  if (Ustrcmp(ipa2->address, ipa->address) == 0) return list;
ipa2 = store_get_perm(sizeof(ip_address_item), FALSE);
*ipa2 = *ipa;
ipa2->next = list;
return ipa2;
}