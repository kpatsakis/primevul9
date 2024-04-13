cgi_initialize_post(void)
{
  char		*content_length,	/* Length of input data (string) */
		*data;			/* Pointer to form data string */
  size_t	length,			/* Length of input data */
		tbytes;			/* Total number of bytes read */
  ssize_t	nbytes;			/* Number of bytes read this read() */
  int		status;			/* Return status */


 /*
  * Check to see if there is anything for us to read...
  */

  content_length = getenv("CONTENT_LENGTH");
  if (content_length == NULL || atoi(content_length) <= 0)
    return (0);

 /*
  * Get the length of the input stream and allocate a buffer for it...
  */

  length = (size_t)strtol(content_length, NULL, 10);
  data   = malloc(length + 1);

  if (data == NULL)
    return (0);

 /*
  * Read the data into the buffer...
  */

  for (tbytes = 0; tbytes < length; tbytes += (size_t)nbytes)
    if ((nbytes = read(0, data + tbytes, (size_t)(length - tbytes))) < 0)
    {
      if (errno != EAGAIN)
      {
        free(data);
        return (0);
      }
      else
        nbytes = 0;
    }
    else if (nbytes == 0)
    {
     /*
      * CUPS STR #3176: OpenBSD: Early end-of-file on POST data causes 100% CPU
      *
      * This should never happen, but does on OpenBSD.  If we see early end-of-
      * file, treat this as an error and process no data.
      */

      free(data);
      return (0);
    }

  data[length] = '\0';

 /*
  * Parse it out...
  */

  status = cgi_initialize_string(data);

 /*
  * Free the data and return...
  */

  free(data);

  return (status);
}