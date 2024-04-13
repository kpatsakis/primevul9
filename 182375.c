static int fits_eval_pixrange (FILE *fp, FITS_HDU_LIST *hdu)

{register int maxelem;
#define FITSNPIX 4096
 unsigned char pixdat[FITSNPIX];
 int nelem, bpp;
 int blank_found = 0, nan_found = 0;

 if (fseek (fp, hdu->data_offset, SEEK_SET) < 0)
   FITS_RETURN ("fits_eval_pixrange: cant position file", -1);

 bpp = hdu->bpp;                  /* Number of bytes per pixel */
 nelem = hdu->udata_size / bpp;   /* Number of data elements */

 switch (hdu->bitpix)
 {
   case 8: {
     register FITS_BITPIX8 pixval;
     register unsigned char *ptr;
     FITS_BITPIX8 minval = 255, maxval = 0;
     FITS_BITPIX8 blankval;

     while (nelem > 0)
     {
       maxelem = sizeof (pixdat)/bpp;
       if (nelem < maxelem) maxelem = nelem;
       nelem -= maxelem;
       if (fread ((char *)pixdat, bpp, maxelem, fp) != maxelem)
         FITS_RETURN ("fits_eval_pixrange: error on read bitpix 8 data", -1);

       ptr = pixdat;
       if (hdu->used.blank)
       {
         blankval = (FITS_BITPIX8)hdu->blank;
         while (maxelem-- > 0)
         {
           pixval = (FITS_BITPIX8)*(ptr++);
           if (pixval != blankval)
           {
             if (pixval < minval) minval = pixval;
             else if (pixval > maxval) maxval = pixval;
           }
           else blank_found = 1;
         }
       }
       else
       {
         while (maxelem-- > 0)
         {
           pixval = (FITS_BITPIX8)*(ptr++);
           if (pixval < minval) minval = pixval;
           else if (pixval > maxval) maxval = pixval;
         }
       }
     }
     hdu->pixmin = minval;
     hdu->pixmax = maxval;
     break; }

   case 16: {
     register FITS_BITPIX16 pixval;
     register unsigned char *ptr;
     FITS_BITPIX16 minval = 0x7fff, maxval = ~0x7fff;

     while (nelem > 0)
     {
       maxelem = sizeof (pixdat)/bpp;
       if (nelem < maxelem) maxelem = nelem;
       nelem -= maxelem;
       if (fread ((char *)pixdat, bpp, maxelem, fp) != maxelem)
         FITS_RETURN ("fits_eval_pixrange: error on read bitpix 16 data", -1);

       ptr = pixdat;
       if (hdu->used.blank)
       {FITS_BITPIX16 blankval = (FITS_BITPIX16)hdu->blank;

         while (maxelem-- > 0)
         {
           FITS_GETBITPIX16 (ptr, pixval);
           ptr += 2;
           if (pixval != blankval)
           {
             if (pixval < minval) minval = pixval;
             else if (pixval > maxval) maxval = pixval;
           }
           else blank_found = 1;
         }
       }
       else
       {
         while (maxelem-- > 0)
         {
           FITS_GETBITPIX16 (ptr, pixval);
           ptr += 2;
           if (pixval < minval) minval = pixval;
           else if (pixval > maxval) maxval = pixval;
         }
       }
     }
     hdu->pixmin = minval;
     hdu->pixmax = maxval;
     break; }


   case 32: {
     register FITS_BITPIX32 pixval;
     register unsigned char *ptr;
     FITS_BITPIX32 minval = 0x7fffffff, maxval = ~0x7fffffff;

     while (nelem > 0)
     {
       maxelem = sizeof (pixdat)/bpp;
       if (nelem < maxelem) maxelem = nelem;
       nelem -= maxelem;
       if (fread ((char *)pixdat, bpp, maxelem, fp) != maxelem)
         FITS_RETURN ("fits_eval_pixrange: error on read bitpix 32 data", -1);

       ptr = pixdat;
       if (hdu->used.blank)
       {FITS_BITPIX32 blankval = (FITS_BITPIX32)hdu->blank;

         while (maxelem-- > 0)
         {
           FITS_GETBITPIX32 (ptr, pixval);
           ptr += 4;
           if (pixval != blankval)
           {
             if (pixval < minval) minval = pixval;
             else if (pixval > maxval) maxval = pixval;
           }
           else blank_found = 1;
         }
       }
       else
       {
         while (maxelem-- > 0)
         {
           FITS_GETBITPIX32 (ptr, pixval);
           ptr += 4;
           if (pixval < minval) minval = pixval;
           else if (pixval > maxval) maxval = pixval;
         }
       }
     }
     hdu->pixmin = minval;
     hdu->pixmax = maxval;
     break; }

   case -32: {
     register FITS_BITPIXM32 pixval;
     register unsigned char *ptr;
     FITS_BITPIXM32 minval, maxval;
     int first = 1;

     /* initialize */

     pixval = 0;
     minval = 0;
     maxval = 0;

     while (nelem > 0)
     {
       maxelem = sizeof (pixdat)/bpp;
       if (nelem < maxelem) maxelem = nelem;
       nelem -= maxelem;
       if (fread ((char *)pixdat, bpp, maxelem, fp) != maxelem)
         FITS_RETURN ("fits_eval_pixrange: error on read bitpix -32 data", -1);

       ptr = pixdat;
       while (maxelem-- > 0)
       {
         if (!fits_nan_32 (ptr))
         {
           FITS_GETBITPIXM32 (ptr, pixval);
           ptr += 4;
           if (first)
           {
             first = 0;
             minval = maxval = pixval;
           }
           else if (pixval < minval) { minval = pixval; }
           else if (pixval > maxval) { maxval = pixval; }
         }
         else nan_found = 1;
       }
     }
     hdu->pixmin = minval;
     hdu->pixmax = maxval;
     break; }

   case -64: {
     register FITS_BITPIXM64 pixval;
     register unsigned char *ptr;
     FITS_BITPIXM64 minval, maxval;
     int first = 1;

     /* initialize */

     minval = 0;
     maxval = 0;

     while (nelem > 0)
     {
       maxelem = sizeof (pixdat)/bpp;
       if (nelem < maxelem) maxelem = nelem;
       nelem -= maxelem;
       if (fread ((char *)pixdat, bpp, maxelem, fp) != maxelem)
         FITS_RETURN ("fits_eval_pixrange: error on read bitpix -64 data", -1);

       ptr = pixdat;
       while (maxelem-- > 0)
       {
         if (!fits_nan_64 (ptr))
         {
           FITS_GETBITPIXM64 (ptr, pixval);
           ptr += 8;
           if (first)
           {
             first = 0;
             minval = maxval = pixval;
           }
           else if (pixval < minval) { minval = pixval; }
           else if (pixval > maxval) { maxval = pixval; }
         }
         else nan_found = 1;
       }
     }
     hdu->pixmin = minval;
     hdu->pixmax = maxval;
     break; }
 }
 if (nan_found) hdu->used.nan_value = 1;
 if (blank_found) hdu->used.blank_value = 1;

 return (0);
}