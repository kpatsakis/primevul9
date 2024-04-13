int fits_to_pgmraw (char *fitsfile, char *pgmfile)

{FITS_FILE *fitsin = NULL;
 FILE *pgmout = NULL;
 FITS_HDU_LIST *hdu;
 FITS_PIX_TRANSFORM trans;
 int retval = -1, nbytes, maxbytes;
 char buffer[1024];

 fitsin = fits_open (fitsfile, "r");  /* Open FITS-file for reading */
 if (fitsin == NULL) goto err_return;

 if (fitsin->n_pic < 1) goto err_return;  /* Any picture in it ? */

 hdu = fits_seek_image (fitsin, 1);       /* Position to the first image */
 if (hdu == NULL) goto err_return;
 if (hdu->naxis < 2) goto err_return;     /* Enough dimensions ? */

 pgmout = g_fopen (pgmfile, "wb");
 if (pgmout == NULL) goto err_return;

                                   /* Write PGM header with width/height */
 fprintf (pgmout, "P5\n%d %d\n255\n", hdu->naxisn[0], hdu->naxisn[1]);

 /* Set up transformation for FITS pixel values to 0...255 */
 /* It maps trans.pixmin to trans.datamin and trans.pixmax to trans.datamax. */
 /* Values out of range [datamin, datamax] are clamped */
 trans.pixmin = hdu->pixmin;
 trans.pixmax = hdu->pixmax;
 trans.datamin = 0.0;
 trans.datamax = 255.0;
 trans.replacement = 0.0;  /* Blank/NaN replacement value */
 trans.dsttyp = 'c';       /* Output type is character */

 nbytes = hdu->naxisn[0]*hdu->naxisn[1];
 while (nbytes > 0)
 {
   maxbytes = sizeof (buffer);
   if (maxbytes > nbytes) maxbytes = nbytes;

   /* Read pixels and transform them */
   if (fits_read_pixel (fitsin, hdu, maxbytes, &trans, buffer) != maxbytes)
     goto err_return;

   if (fwrite (buffer, 1, maxbytes, pgmout) != maxbytes)
     goto err_return;

   nbytes -= maxbytes;
 }
 retval = 0;

err_return:

 if (fitsin) fits_close (fitsin);
 if (pgmout) fclose (pgmout);

 return (retval);
}