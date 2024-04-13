strdupWrapper (const char *string)
{
  char *address = strdup (string);
  if (!address)
    outOfMemory ();
  return address;
}