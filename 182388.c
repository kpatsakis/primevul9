FITS_HDU_LIST *fits_add_hdu (FITS_FILE *ff)

{FITS_HDU_LIST *newhdu, *hdu;

 if (ff->openmode != 'w')
   FITS_RETURN ("fits_add_hdu: file not open for writing", NULL);

 newhdu = fits_new_hdulist ();
 if (newhdu == NULL) return (NULL);

 if (ff->hdu_list == NULL)
 {
   ff->hdu_list = newhdu;
 }
 else
 {
   hdu = ff->hdu_list;
   while (hdu->next_hdu != NULL)
     hdu = hdu->next_hdu;
   hdu->next_hdu = newhdu;
 }

 return (newhdu);
}