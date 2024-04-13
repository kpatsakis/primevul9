sub_points_edwards (mpi_point_t result,
                    mpi_point_t p1, mpi_point_t p2,
                    mpi_ec_t ctx)
{
  mpi_point_t p2i = _gcry_mpi_point_new (0);
  point_set (p2i, p2);
  _gcry_mpi_neg (p2i->x, p2i->x);
  add_points_edwards (result, p1, p2i, ctx);
  _gcry_mpi_point_release (p2i);
}