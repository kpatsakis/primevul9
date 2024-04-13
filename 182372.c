static FITS_FILE *fits_new_filestruct (void)

{FITS_FILE *ff;

 ff = (FITS_FILE *)malloc (sizeof (FITS_FILE));
 if (ff == NULL) return (NULL);

 memset ((char *)ff, 0, sizeof (*ff));
 return (ff);
}