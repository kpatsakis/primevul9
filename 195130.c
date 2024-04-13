safe_strcpy (char *dest, const char *src, size_t maxlength)
{
size_t len;

if (!dest)
  {
  DEBUG_X (0, ("ERROR: NULL dest in safe_strcpy\n"));
  return NULL;
  }

if (!src)
  {
  *dest = 0;
  return dest;
  }

len = strlen (src);

if (len > maxlength)
  {
  DEBUG_X (0, ("ERROR: string overflow by %d in safe_strcpy [%.50s]\n",
	    (int) (len - maxlength), src));
  len = maxlength;
  }

memcpy (dest, src, len);
dest[len] = 0;
return dest;
}