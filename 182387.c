static FITS_HDU_LIST *fits_new_hdulist (void)

{FITS_HDU_LIST *hdl;

 hdl = (FITS_HDU_LIST *)malloc (sizeof (FITS_HDU_LIST));
 if (hdl == NULL) return (NULL);

 memset ((char *)hdl, 0, sizeof (*hdl));
 hdl->pixmin = hdl->pixmax = hdl->datamin = hdl->datamax = 0.0;
 hdl->bzero = hdl->bscale = 0.0;

 return (hdl);
}