static void fits_set_error (const char *errmsg)

{
  if (fits_n_error < FITS_MAX_ERROR)
  {
    strncpy (fits_error[fits_n_error], errmsg, FITS_ERROR_LENGTH);
    fits_error[fits_n_error++][FITS_ERROR_LENGTH-1] = '\0';
  }
}