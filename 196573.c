grub_burn_stack (grub_size_t size)
{
  char buf[64];

  grub_memset (buf, 0, sizeof (buf));
  if (size > sizeof (buf))
    grub_burn_stack (size - sizeof (buf));
}