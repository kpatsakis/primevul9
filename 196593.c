grub_crypto_lookup_md_by_name (const char *name)
{
  const gcry_md_spec_t *md;
  int first = 1;
  while (1)
    {
      for (md = grub_digests; md; md = md->next)
	if (grub_strcasecmp (name, md->name) == 0)
	  return md;
      if (grub_crypto_autoload_hook && first)
	grub_crypto_autoload_hook (name);
      else
	return NULL;
      first = 0;
    }
}