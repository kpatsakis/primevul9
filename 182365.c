FITS_HDU_LIST *fits_image_info (FITS_FILE *ff, int picind, int *hdupicind)

{FITS_HDU_LIST *hdulist;
 int firstpic, lastpic;

 if (ff == NULL)
   FITS_RETURN ("fits_image_info: ff is NULL", NULL);

 if (ff->openmode != 'r')
   FITS_RETURN ("fits_image_info: file not open for reading", NULL);

 if ((picind < 1) || (picind > ff->n_pic))
   FITS_RETURN ("fits_image_info: picind out of range", NULL);

 firstpic = 1;
 for (hdulist = ff->hdu_list; hdulist != NULL; hdulist = hdulist->next_hdu)
 {
   if (hdulist->numpic <= 0) continue;
   lastpic = firstpic+hdulist->numpic-1;
   if (picind <= lastpic)  /* Found image in current HDU ? */
     break;

   firstpic = lastpic+1;
 }
 *hdupicind = picind - firstpic + 1;
 return (hdulist);
}