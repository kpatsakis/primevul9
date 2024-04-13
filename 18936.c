detached_get_signatures(ns_detached_signatures_t *sigs,
                        const char *flavor_name)
{
  smartlist_t *sl = strmap_get(sigs->signatures, flavor_name);
  if (!sl) {
    sl = smartlist_create();
    strmap_set(sigs->signatures, flavor_name, sl);
  }
  return sl;
}