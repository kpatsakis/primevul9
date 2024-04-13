static void fits_delete_filestruct (FITS_FILE *ff)

{
 if (ff == NULL) return;

 fits_delete_hdulist (ff->hdu_list);
 ff->hdu_list = NULL;

 ff->fp = NULL;
 free ((char *)ff);
}