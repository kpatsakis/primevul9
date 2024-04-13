char *fits_get_error (void)

{static char errmsg[FITS_ERROR_LENGTH];
 int k;

 if (fits_n_error <= 0) return (NULL);
 strcpy (errmsg, fits_error[0]);

 for (k = 1; k < fits_n_error; k++)
   strcpy (fits_error[k-1], fits_error[k]);

 fits_n_error--;

 return (errmsg);
}