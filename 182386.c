FITS_HDU_LIST *fits_seek_image (FITS_FILE *ff, int picind)

{FITS_HDU_LIST *hdulist;
 int hdupicind;
 long offset, pic_size;

 hdulist = fits_image_info (ff, picind, &hdupicind);
 if (hdulist == NULL) return (NULL);

 pic_size = hdulist->bpp * hdulist->naxisn[0] * hdulist->naxisn[1];
 offset = hdulist->data_offset + (hdupicind-1)*pic_size;
 if (fseek (ff->fp, offset, SEEK_SET) < 0)
   FITS_RETURN ("fits_seek_image: Unable to position to image", NULL);

 return (hdulist);
}