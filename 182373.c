FITS_FILE *fits_open (const char *filename, const char *openmode)

{int reading, writing, n_rec, n_hdr;
 long fpos_header, fpos_data;
 FILE *fp;
 FITS_FILE *ff;
 FITS_RECORD_LIST *hdrlist;
 FITS_HDU_LIST *hdulist, *last_hdulist;

 /* initialize */

 hdulist = NULL;
 last_hdulist = NULL;

 /* Check the IEEE-format we are running on */
 {float one32 = 1.0;
  double one64 = 1.0;
  unsigned char *op32 = (unsigned char *)&one32;
  unsigned char *op64 = (unsigned char *)&one64;

  if (sizeof (float) == 4)
  {
    fits_ieee32_intel = (op32[3] == 0x3f);
    fits_ieee32_motorola = (op32[0] == 0x3f);
  }
  if (sizeof (double) == 8)
  {
    fits_ieee64_intel = (op64[7] == 0x3f);
    fits_ieee64_motorola = (op64[0] == 0x3f);
  }
 }

 if ((filename == NULL) || (*filename == '\0') || (openmode == NULL))
   FITS_RETURN ("fits_open: Invalid parameters", NULL);

 reading = (strcmp (openmode, "r") == 0);
 writing = (strcmp (openmode, "w") == 0);
 if ((!reading) && (!writing))
   FITS_RETURN ("fits_open: Invalid openmode", NULL);

 fp = g_fopen (filename, reading ? "rb" : "wb");
 if (fp == NULL) FITS_RETURN ("fits_open: fopen() failed", NULL);

 ff = fits_new_filestruct ();
 if (ff == NULL)
 {
   fclose (fp);
   FITS_RETURN ("fits_open: No more memory", NULL);
 }

 ff->fp = fp;
 ff->openmode = *openmode;

 if (writing) return (ff);

 for (n_hdr = 0; ; n_hdr++)   /* Read through all HDUs */
 {
   fpos_header = ftell (fp);    /* Save file position of header */
   hdrlist = fits_read_header (fp, &n_rec);

   if (hdrlist == NULL)
   {
     if (n_hdr > 0)        /* At least one header must be present. */
       fits_drop_error (); /* If we got a header already, drop the error */
     break;
   }
   fpos_data = ftell (fp);      /* Save file position of data */

                           /* Decode the header */
   hdulist = fits_decode_header (hdrlist, fpos_header, fpos_data);
   if (hdulist == NULL)
   {
     fits_delete_recordlist (hdrlist);
     break;
   }
   ff->n_hdu++;
   ff->n_pic += hdulist->numpic;

   if (hdulist->used.blank_value) ff->blank_used = 1;
   if (hdulist->used.nan_value) ff->nan_used = 1;

   if (n_hdr == 0)
     ff->hdu_list = hdulist;
   else
     last_hdulist->next_hdu = hdulist;
   last_hdulist = hdulist;
                           /* Evaluate the range of pixel data */
   fits_eval_pixrange (fp, hdulist);

   /* Reposition to start of next header */
   if (fseek (fp, hdulist->data_offset+hdulist->data_size, SEEK_SET) < 0)
     break;
 }

 return (ff);
}