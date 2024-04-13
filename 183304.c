main (int argc, char **argv)
{
  FILE *fp;
  struct stat st;
  unsigned char *buf;
  size_t buflen;
  gcry_mpi_t *result;
  int badpass;

  if (argc != 3)
    {
      fprintf (stderr, "usage: testp12 file passphrase\n");
      return 1;
    }

  gcry_control (GCRYCTL_DISABLE_SECMEM, NULL);
  gcry_control (GCRYCTL_INITIALIZATION_FINISHED, NULL);

  fp = fopen (argv[1], "rb");
  if (!fp)
    {
      fprintf (stderr, "can't open '%s': %s\n", argv[1], strerror (errno));
      return 1;
    }

  if (fstat (fileno(fp), &st))
    {
      fprintf (stderr, "can't stat '%s': %s\n", argv[1], strerror (errno));
      return 1;
    }

  buflen = st.st_size;
  buf = gcry_malloc (buflen+1);
  if (!buf || fread (buf, buflen, 1, fp) != 1)
    {
      fprintf (stderr, "error reading '%s': %s\n", argv[1], strerror (errno));
      return 1;
    }
  fclose (fp);

  result = p12_parse (buf, buflen, argv[2], cert_cb, NULL, &badpass);
  if (result)
    {
      int i, rc;
      unsigned char *tmpbuf;

      for (i=0; result[i]; i++)
        {
          rc = gcry_mpi_aprint (GCRYMPI_FMT_HEX, &tmpbuf,
                                NULL, result[i]);
          if (rc)
            printf ("%d: [error printing number: %s]\n",
                    i, gpg_strerror (rc));
          else
            {
              printf ("%d: %s\n", i, tmpbuf);
              gcry_free (tmpbuf);
            }
        }
    }

  return 0;

}