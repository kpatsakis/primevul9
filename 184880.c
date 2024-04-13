_gcry_mpi_ec_curve_point (gcry_mpi_point_t point, mpi_ec_t ctx)
{
  int res = 0;
  gcry_mpi_t x, y, w;

  x = mpi_new (0);
  y = mpi_new (0);
  w = mpi_new (0);

  switch (ctx->model)
    {
    case MPI_EC_WEIERSTRASS:
      {
        gcry_mpi_t xxx = mpi_new (0);

        if (_gcry_mpi_ec_get_affine (x, y, point, ctx))
          return 0;

        /* y^2 == x^3 + a·x + b */
        ec_pow2 (y, y, ctx);

        ec_pow3 (xxx, x, ctx);
        ec_mulm (w, ctx->a, x, ctx);
        ec_addm (w, w, ctx->b, ctx);
        ec_addm (w, w, xxx, ctx);

        if (!mpi_cmp (y, w))
          res = 1;

        _gcry_mpi_release (xxx);
      }
      break;
    case MPI_EC_MONTGOMERY:
      {
#define xx y
        /* With Montgomery curve, only X-coordinate is valid.  */
        if (_gcry_mpi_ec_get_affine (x, NULL, point, ctx))
          return 0;

        /* The equation is: b * y^2 == x^3 + a · x^2 + x */
        /* We check if right hand is quadratic residue or not by
           Euler's criterion.  */
        /* CTX->A has (a-2)/4 and CTX->B has b^-1 */
        ec_mulm (w, ctx->a, mpi_const (MPI_C_FOUR), ctx);
        ec_addm (w, w, mpi_const (MPI_C_TWO), ctx);
        ec_mulm (w, w, x, ctx);
        ec_pow2 (xx, x, ctx);
        ec_addm (w, w, xx, ctx);
        ec_addm (w, w, mpi_const (MPI_C_ONE), ctx);
        ec_mulm (w, w, x, ctx);
        ec_mulm (w, w, ctx->b, ctx);
#undef xx
        /* Compute Euler's criterion: w^(p-1)/2 */
#define p_minus1 y
        ec_subm (p_minus1, ctx->p, mpi_const (MPI_C_ONE), ctx);
        mpi_rshift (p_minus1, p_minus1, 1);
        ec_powm (w, w, p_minus1, ctx);

        res = !mpi_cmp_ui (w, 1);
#undef p_minus1
      }
      break;
    case MPI_EC_EDWARDS:
      {
        if (_gcry_mpi_ec_get_affine (x, y, point, ctx))
          return 0;

        /* a · x^2 + y^2 - 1 - b · x^2 · y^2 == 0 */
        ec_pow2 (x, x, ctx);
        ec_pow2 (y, y, ctx);
        if (ctx->dialect == ECC_DIALECT_ED25519)
          {
            mpi_set (w, x);
            _gcry_mpi_neg (w, w);
          }
        else
          ec_mulm (w, ctx->a, x, ctx);
        ec_addm (w, w, y, ctx);
        ec_subm (w, w, mpi_const (MPI_C_ONE), ctx);
        ec_mulm (x, x, y, ctx);
        ec_mulm (x, x, ctx->b, ctx);
        ec_subm (w, w, x, ctx);
        if (!mpi_cmp_ui (w, 0))
          res = 1;
      }
      break;
    }

  _gcry_mpi_release (w);
  _gcry_mpi_release (x);
  _gcry_mpi_release (y);

  return res;
}