dup_point_edwards (mpi_point_t result, mpi_point_t point, mpi_ec_t ctx)
{
#define X1 (point->x)
#define Y1 (point->y)
#define Z1 (point->z)
#define X3 (result->x)
#define Y3 (result->y)
#define Z3 (result->z)
#define B (ctx->t.scratch[0])
#define C (ctx->t.scratch[1])
#define D (ctx->t.scratch[2])
#define E (ctx->t.scratch[3])
#define F (ctx->t.scratch[4])
#define H (ctx->t.scratch[5])
#define J (ctx->t.scratch[6])

  /* Compute: (X_3 : Y_3 : Z_3) = 2( X_1 : Y_1 : Z_1 ) */

  /* B = (X_1 + Y_1)^2  */
  ec_addm (B, X1, Y1, ctx);
  ec_pow2 (B, B, ctx);

  /* C = X_1^2 */
  /* D = Y_1^2 */
  ec_pow2 (C, X1, ctx);
  ec_pow2 (D, Y1, ctx);

  /* E = aC */
  if (ctx->dialect == ECC_DIALECT_ED25519)
    {
      mpi_set (E, C);
      _gcry_mpi_neg (E, E);
    }
  else
    ec_mulm (E, ctx->a, C, ctx);

  /* F = E + D */
  ec_addm (F, E, D, ctx);

  /* H = Z_1^2 */
  ec_pow2 (H, Z1, ctx);

  /* J = F - 2H */
  ec_mul2 (J, H, ctx);
  ec_subm (J, F, J, ctx);

  /* X_3 = (B - C - D) · J */
  ec_subm (X3, B, C, ctx);
  ec_subm (X3, X3, D, ctx);
  ec_mulm (X3, X3, J, ctx);

  /* Y_3 = F · (E - D) */
  ec_subm (Y3, E, D, ctx);
  ec_mulm (Y3, Y3, F, ctx);

  /* Z_3 = F · J */
  ec_mulm (Z3, F, J, ctx);

#undef X1
#undef Y1
#undef Z1
#undef X3
#undef Y3
#undef Z3
#undef B
#undef C
#undef D
#undef E
#undef F
#undef H
#undef J
}