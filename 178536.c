cgi_initialize_multipart(
    const char *boundary)		/* I - Boundary string */
{
  char		line[10240],		/* MIME header line */
		name[1024],		/* Form variable name */
		filename[1024],		/* Form filename */
		mimetype[1024],		/* MIME media type */
		bstring[256],		/* Boundary string to look for */
		*ptr,			/* Pointer into name/filename */
		*end;			/* End of buffer */
  int		ch,			/* Character from file */
		fd;			/* Temporary file descriptor */
  size_t	blen;			/* Length of boundary string */


 /*
  * Read multipart form data until we run out...
  */

  name[0]     = '\0';
  filename[0] = '\0';
  mimetype[0] = '\0';

  snprintf(bstring, sizeof(bstring), "\r\n--%s", boundary);
  blen = strlen(bstring);

  while (fgets(line, sizeof(line), stdin))
  {
    if (!strcmp(line, "\r\n"))
    {
     /*
      * End of headers, grab value...
      */

      if (filename[0])
      {
       /*
        * Read an embedded file...
	*/

        if (form_file)
	{
	 /*
	  * Remove previous file...
	  */

	  cgi_unlink_file();
	}

       /*
        * Allocate memory for the new file...
	*/

	if ((form_file = calloc(1, sizeof(cgi_file_t))) == NULL)
	  return (0);

        form_file->name     = strdup(name);
	form_file->filename = strdup(filename);
	form_file->mimetype = strdup(mimetype);

        fd = cupsTempFd(form_file->tempfile, sizeof(form_file->tempfile));

        if (fd < 0)
	  return (0);

        atexit(cgi_unlink_file);

       /*
        * Copy file data to the temp file...
	*/

        ptr = line;

	while ((ch = getchar()) != EOF)
	{
	  *ptr++ = (char)ch;

          if ((size_t)(ptr - line) >= blen && !memcmp(ptr - blen, bstring, blen))
	  {
	    ptr -= blen;
	    break;
	  }

          if ((ptr - line - (int)blen) >= 8192)
	  {
	   /*
	    * Write out the first 8k of the buffer...
	    */

	    write(fd, line, 8192);
	    memmove(line, line + 8192, (size_t)(ptr - line - 8192));
	    ptr -= 8192;
	  }
	}

       /*
        * Write the rest of the data and close the temp file...
	*/

	if (ptr > line)
          write(fd, line, (size_t)(ptr - line));

	close(fd);
      }
      else
      {
       /*
        * Just get a form variable; the current code only handles
	* form values up to 10k in size...
	*/

        ptr = line;
	end = line + sizeof(line) - 1;

	while ((ch = getchar()) != EOF)
	{
	  if (ptr < end)
	    *ptr++ = (char)ch;

          if ((size_t)(ptr - line) >= blen && !memcmp(ptr - blen, bstring, blen))
	  {
	    ptr -= blen;
	    break;
	  }
	}

	*ptr = '\0';

       /*
        * Set the form variable...
	*/

	if ((ptr = strrchr(name, '-')) != NULL && isdigit(ptr[1] & 255))
	{
	 /*
	  * Set a specific index in the array...
	  */

	  *ptr++ = '\0';
	  if (line[0])
            cgiSetArray(name, atoi(ptr) - 1, line);
	}
	else if (cgiGetVariable(name))
	{
	 /*
	  * Add another element in the array...
	  */

	  cgiSetArray(name, cgiGetSize(name), line);
	}
	else
	{
	 /*
	  * Just set the line...
	  */

	  cgiSetVariable(name, line);
	}
      }

     /*
      * Read the rest of the current line...
      */

      fgets(line, sizeof(line), stdin);

     /*
      * Clear the state vars...
      */

      name[0]     = '\0';
      filename[0] = '\0';
      mimetype[0] = '\0';
    }
    else if (!_cups_strncasecmp(line, "Content-Disposition:", 20))
    {
      if ((ptr = strstr(line + 20, " name=\"")) != NULL)
      {
        strlcpy(name, ptr + 7, sizeof(name));

	if ((ptr = strchr(name, '\"')) != NULL)
	  *ptr = '\0';
      }

      if ((ptr = strstr(line + 20, " filename=\"")) != NULL)
      {
        strlcpy(filename, ptr + 11, sizeof(filename));

	if ((ptr = strchr(filename, '\"')) != NULL)
	  *ptr = '\0';
      }
    }
    else if (!_cups_strncasecmp(line, "Content-Type:", 13))
    {
      for (ptr = line + 13; isspace(*ptr & 255); ptr ++);

      strlcpy(mimetype, ptr, sizeof(mimetype));

      for (ptr = mimetype + strlen(mimetype) - 1;
           ptr > mimetype && isspace(*ptr & 255);
	   *ptr-- = '\0');
    }
  }

 /*
  * Return 1 for "form data found"...
  */

  return (1);
}