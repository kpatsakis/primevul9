grub_crypto_lookup_cipher_by_name (const char *name)
{
  const gcry_cipher_spec_t *ciph;
  int first = 1;
  while (1)
    {
      for (ciph = grub_ciphers; ciph; ciph = ciph->next)
	{
	  const char **alias;
	  if (grub_strcasecmp (name, ciph->name) == 0)
	    return ciph;
	  if (!ciph->aliases)
	    continue;
	  for (alias = ciph->aliases; *alias; alias++)
	    if (grub_strcasecmp (name, *alias) == 0)
	      return ciph;
	}
      if (grub_crypto_autoload_hook && first)
	grub_crypto_autoload_hook (name);
      else
	return NULL;
      first = 0;
    }
}