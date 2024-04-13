aubio_filterbank_set_coeffs (aubio_filterbank_t * f, const fmat_t * filter_coeffs)
{
  fmat_copy(filter_coeffs, f->filters);
  return 0;
}