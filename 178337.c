_gcry_ecc_eddsa_recover_x (gcry_mpi_t x, gcry_mpi_t y, int sign, mpi_ec_t ec)
{
  gpg_err_code_t rc = 0;
  gcry_mpi_t u, v, v3, t;
  static gcry_mpi_t p58, seven;

  if (ec->dialect != ECC_DIALECT_ED25519)
    return GPG_ERR_NOT_IMPLEMENTED;

  if (!p58)
    p58 = scanval ("0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                   "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD");
  if (!seven)
    seven = mpi_set_ui (NULL, 7);

  u   = mpi_new (0);
  v   = mpi_new (0);
  v3  = mpi_new (0);
  t   = mpi_new (0);

  /* Compute u and v */
  /* u = y^2    */
  mpi_mulm (u, y, y, ec->p);
  /* v = b*y^2   */
  mpi_mulm (v, ec->b, u, ec->p);
  /* u = y^2-1  */
  mpi_sub_ui (u, u, 1);
  /* v = b*y^2+1 */
  mpi_add_ui (v, v, 1);

  /* Compute sqrt(u/v) */
  /* v3 = v^3 */
  mpi_powm (v3, v, mpi_const (MPI_C_THREE), ec->p);
  /* t = v3 * v3 * u * v = u * v^7 */
  mpi_powm (t, v, seven, ec->p);
  mpi_mulm (t, t, u, ec->p);
  /* t = t^((p-5)/8) = (u * v^7)^((p-5)/8)  */
  mpi_powm (t, t, p58, ec->p);
  /* x = t * u * v^3 = (u * v^3) * (u * v^7)^((p-5)/8) */
  mpi_mulm (t, t, u, ec->p);
  mpi_mulm (x, t, v3, ec->p);

  /* Adjust if needed.  */
  /* t = v * x^2  */
  mpi_mulm (t, x, x, ec->p);
  mpi_mulm (t, t, v, ec->p);
  /* -t == u ? x = x * sqrt(-1) */
  mpi_sub (t, ec->p, t);
  if (!mpi_cmp (t, u))
    {
      static gcry_mpi_t m1;  /* Fixme: this is not thread-safe.  */
      if (!m1)
        m1 = scanval ("2B8324804FC1DF0B2B4D00993DFBD7A7"
                      "2F431806AD2FE478C4EE1B274A0EA0B0");
      mpi_mulm (x, x, m1, ec->p);
      /* t = v * x^2  */
      mpi_mulm (t, x, x, ec->p);
      mpi_mulm (t, t, v, ec->p);
      /* -t == u ? x = x * sqrt(-1) */
      mpi_sub (t, ec->p, t);
      if (!mpi_cmp (t, u))
        rc = GPG_ERR_INV_OBJ;
    }

  /* Choose the desired square root according to parity */
  if (mpi_test_bit (x, 0) != !!sign)
    mpi_sub (x, ec->p, x);

  mpi_free (t);
  mpi_free (v3);
  mpi_free (v);
  mpi_free (u);

  return rc;
}