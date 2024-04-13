int fits_read_pixel (FITS_FILE *ff, FITS_HDU_LIST *hdulist, int npix,
                     FITS_PIX_TRANSFORM *trans, void *buf)

{double offs, scale;
 double datadiff, pixdiff;
 unsigned char pixbuffer[4096], *pix, *cdata;
 unsigned char creplace;
 int transcount = 0;
 long tdata, tmin, tmax;
 int maxelem;
 FITS_BITPIX8 bp8, bp8blank;
 FITS_BITPIX16 bp16, bp16blank;
 FITS_BITPIX32 bp32, bp32blank;
 FITS_BITPIXM32 bpm32;
 FITS_BITPIXM64 bpm64;

 /* initialize */

 bpm32 = 0;

 if (ff->openmode != 'r') return (-1);   /* Not open for reading */
 if (trans->dsttyp != 'c') return (-1);  /* Currently we only return chars */
 if (npix <= 0) return (npix);

 datadiff = trans->datamax - trans->datamin;
 pixdiff = trans->pixmax - trans->pixmin;

 offs = trans->datamin - trans->pixmin*datadiff/pixdiff;
 scale = datadiff / pixdiff;

 tmin = (long)trans->datamin;
 tmax = (long)trans->datamax;
 if (tmin < 0) tmin = 0; else if (tmin > 255) tmin = 255;
 if (tmax < 0) tmax = 0; else if (tmax > 255) tmax = 255;

 cdata = (unsigned char *)buf;
 creplace = (unsigned char)trans->replacement;

 switch (hdulist->bitpix)
 {
   case 8:
     while (npix > 0)  /* For all pixels to read */
     {
       maxelem = sizeof (pixbuffer) / hdulist->bpp;
       if (maxelem > npix) maxelem = npix;
       if (fread ((char *)pixbuffer, hdulist->bpp, maxelem, ff->fp) != maxelem)
         return (-1);
       npix -= maxelem;

       pix = pixbuffer;
       if (hdulist->used.blank)
       {
         bp8blank = (FITS_BITPIX8)hdulist->blank;
         while (maxelem--)
         {
           bp8 = (FITS_BITPIX8)*(pix++);
           if (bp8 == bp8blank)      /* Is it a blank pixel ? */
             *(cdata++) = creplace;
           else                      /* Do transform */
           {
             tdata = (long)(bp8 * scale + offs);
             if (tdata < tmin) tdata = tmin;
             else if (tdata > tmax) tdata = tmax;
             *(cdata++) = (unsigned char)tdata;
           }
           transcount++;
         }
       }
       else
       {
         while (maxelem--)
         {
           bp8 = (FITS_BITPIX8)*(pix++);
           tdata = (long)(bp8 * scale + offs);
           if (tdata < tmin) tdata = tmin;
           else if (tdata > tmax) tdata = tmax;
           *(cdata++) = (unsigned char)tdata;
           transcount++;
         }
       }
     }
     break;

   case 16:
     while (npix > 0)  /* For all pixels to read */
     {
       maxelem = sizeof (pixbuffer) / hdulist->bpp;
       if (maxelem > npix) maxelem = npix;
       if (fread ((char *)pixbuffer, hdulist->bpp, maxelem, ff->fp) != maxelem)
         return (-1);
       npix -= maxelem;

       pix = pixbuffer;
       if (hdulist->used.blank)
       {
         bp16blank = (FITS_BITPIX16)hdulist->blank;
         while (maxelem--)
         {
           FITS_GETBITPIX16 (pix, bp16);
           if (bp16 == bp16blank)
             *(cdata++) = creplace;
           else
           {
             tdata = (long)(bp16 * scale + offs);
             if (tdata < tmin) tdata = tmin;
             else if (tdata > tmax) tdata = tmax;
             *(cdata++) = (unsigned char)tdata;
           }
           transcount++;
           pix += 2;
         }
       }
       else
       {
         while (maxelem--)
         {
           FITS_GETBITPIX16 (pix, bp16);
           tdata = (long)(bp16 * scale + offs);
           if (tdata < tmin) tdata = tmin;
           else if (tdata > tmax) tdata = tmax;
           *(cdata++) = (unsigned char)tdata;
           transcount++;
           pix += 2;
         }
       }
     }
     break;

   case 32:
     while (npix > 0)  /* For all pixels to read */
     {
       maxelem = sizeof (pixbuffer) / hdulist->bpp;
       if (maxelem > npix) maxelem = npix;
       if (fread ((char *)pixbuffer, hdulist->bpp, maxelem, ff->fp) != maxelem)
         return (-1);
       npix -= maxelem;

       pix = pixbuffer;
       if (hdulist->used.blank)
       {
         bp32blank = (FITS_BITPIX32)hdulist->blank;
         while (maxelem--)
         {
           FITS_GETBITPIX32 (pix, bp32);
           if (bp32 == bp32blank)
             *(cdata++) = creplace;
           else
           {
             tdata = (long)(bp32 * scale + offs);
             if (tdata < tmin) tdata = tmin;
             else if (tdata > tmax) tdata = tmax;
             *(cdata++) = (unsigned char)tdata;
           }
           transcount++;
           pix += 4;
         }
       }
       else
       {
         while (maxelem--)
         {
           FITS_GETBITPIX32 (pix, bp32);
           tdata = (long)(bp32 * scale + offs);
           if (tdata < tmin) tdata = tmin;
           else if (tdata > tmax) tdata = tmax;
           *(cdata++) = (unsigned char)tdata;
           transcount++;
           pix += 4;
         }
       }
     }
     break;

   case -32:
     while (npix > 0)  /* For all pixels to read */
     {
       maxelem = sizeof (pixbuffer) / hdulist->bpp;
       if (maxelem > npix) maxelem = npix;
       if (fread ((char *)pixbuffer, hdulist->bpp, maxelem, ff->fp) != maxelem)
         return (-1);
       npix -= maxelem;

       pix = pixbuffer;
       while (maxelem--)
       {
         if (fits_nan_32 (pix))    /* An IEEE special value ? */
           *(cdata++) = creplace;
         else                      /* Do transform */
         {
           FITS_GETBITPIXM32 (pix, bpm32);
           tdata = (long)(bpm32 * scale + offs);
           if (tdata < tmin) tdata = tmin;
           else if (tdata > tmax) tdata = tmax;
           *(cdata++) = (unsigned char)tdata;
         }
         transcount++;
         pix += 4;
       }
     }
     break;

   case -64:
     while (npix > 0)  /* For all pixels to read */
     {
       maxelem = sizeof (pixbuffer) / hdulist->bpp;
       if (maxelem > npix) maxelem = npix;
       if (fread ((char *)pixbuffer, hdulist->bpp, maxelem, ff->fp) != maxelem)
         return (-1);
       npix -= maxelem;

       pix = pixbuffer;
       while (maxelem--)
       {
         if (fits_nan_64 (pix))
           *(cdata++) = creplace;
         else
         {
           FITS_GETBITPIXM64 (pix, bpm64);
           tdata = (long)(bpm64 * scale + offs);
           if (tdata < tmin) tdata = tmin;
           else if (tdata > tmax) tdata = tmax;
           *(cdata++) = (unsigned char)tdata;
         }
         transcount++;
         pix += 8;
       }
     }
     break;
 }
 return (transcount);
}