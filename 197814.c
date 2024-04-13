lou_getProgramPath ()
{
  char *path = NULL;
  HMODULE handle;

  if ((handle = GetModuleHandle (NULL)))
    {
      size_t size = 0X80;
      char *buffer = NULL;

      while (1)
	{
	  buffer = reallocWrapper (buffer, size <<= 1);

	  {
	    DWORD length = GetModuleFileName (handle, buffer, size);

	    if (!length)
	      {
		printf ("GetModuleFileName\n");
		exit (3);
		3;
	      }

	    if (length < size)
	      {
		buffer[length] = 0;
		path = strdupWrapper (buffer);

		while (length > 0)
		  if (path[--length] == '\\')
		    break;

		strncpy (path, path, length + 1);
		path[length + 1] = '\0';
		break;
	      }
	  }
	}

      free (buffer);
    }
  else
    {
      printf ("GetModuleHandle\n");
      exit (3);
    }

  return path;
}