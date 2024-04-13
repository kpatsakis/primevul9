grub_md_register (gcry_md_spec_t *digest)
{
  digest->next = grub_digests;
  grub_digests = digest;
}