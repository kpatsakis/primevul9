decrypt_block (const void *ciphertext, unsigned char *plaintext, size_t length,
               char *salt, size_t saltlen,
               int iter, const void *iv, size_t ivlen,
               const char *pw, int cipher_algo,
               int (*check_fnc) (const void *, size_t))
{
  static const char * const charsets[] = {
    "",   /* No conversion - use the UTF-8 passphrase direct.  */
    "ISO-8859-1",
    "ISO-8859-15",
    "ISO-8859-2",
    "ISO-8859-3",
    "ISO-8859-4",
    "ISO-8859-5",
    "ISO-8859-6",
    "ISO-8859-7",
    "ISO-8859-8",
    "ISO-8859-9",
    "KOI8-R",
    "IBM437",
    "IBM850",
    "EUC-JP",
    "BIG5",
    NULL
  };
  int charsetidx = 0;
  char *convertedpw = NULL;   /* Malloced and converted password or NULL.  */
  size_t convertedpwsize = 0; /* Allocated length.  */

  for (charsetidx=0; charsets[charsetidx]; charsetidx++)
    {
      if (*charsets[charsetidx])
        {
          jnlib_iconv_t cd;
          const char *inptr;
          char *outptr;
          size_t inbytes, outbytes;

          if (!convertedpw)
            {
              /* We assume one byte encodings.  Thus we can allocate
                 the buffer of the same size as the original
                 passphrase; the result will actually be shorter
                 then.  */
              convertedpwsize = strlen (pw) + 1;
              convertedpw = gcry_malloc_secure (convertedpwsize);
              if (!convertedpw)
                {
                  log_info ("out of secure memory while"
                            " converting passphrase\n");
                  break; /* Give up.  */
                }
            }

          cd = jnlib_iconv_open (charsets[charsetidx], "utf-8");
          if (cd == (jnlib_iconv_t)(-1))
            continue;

          inptr = pw;
          inbytes = strlen (pw);
          outptr = convertedpw;
          outbytes = convertedpwsize - 1;
          if ( jnlib_iconv (cd, (const char **)&inptr, &inbytes,
                      &outptr, &outbytes) == (size_t)-1)
            {
              jnlib_iconv_close (cd);
              continue;
            }
          *outptr = 0;
          jnlib_iconv_close (cd);
          log_info ("decryption failed; trying charset '%s'\n",
                    charsets[charsetidx]);
        }
      memcpy (plaintext, ciphertext, length);
      crypt_block (plaintext, length, salt, saltlen, iter, iv, ivlen,
                   convertedpw? convertedpw:pw, cipher_algo, 0);
      if (check_fnc (plaintext, length))
        break; /* Decryption succeeded. */
    }
  gcry_free (convertedpw);
}