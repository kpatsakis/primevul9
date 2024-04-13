void fits_close (FITS_FILE *ff)

{
 if (ff == NULL) FITS_VRETURN ("fits_close: Invalid parameter");

 fclose (ff->fp);

 fits_delete_filestruct (ff);
}