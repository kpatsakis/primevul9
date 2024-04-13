elg_get_nbits (gcry_sexp_t parms)
{
  gcry_sexp_t l1;
  gcry_mpi_t p;
  unsigned int nbits;

  l1 = sexp_find_token (parms, "p", 1);
  if (!l1)
    return 0; /* Parameter P not found.  */

  p= sexp_nth_mpi (l1, 1, GCRYMPI_FMT_USG);
  sexp_release (l1);
  nbits = p? mpi_get_nbits (p) : 0;
  _gcry_mpi_release (p);
  return nbits;
}