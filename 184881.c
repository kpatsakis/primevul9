ec_subm (gcry_mpi_t w, gcry_mpi_t u, gcry_mpi_t v, mpi_ec_t ec)
{
  (void)ec;
  mpi_sub (w, u, v);
  /*ec_mod (w, ec);*/
}