aubio_filterbank_do (aubio_filterbank_t * f, const cvec_t * in, fvec_t * out)
{
  /* apply filter to all input channel, provided out has enough channels */
  //uint_t max_filters = MIN (f->n_filters, out->length);
  //uint_t max_length = MIN (in->length, f->filters->length);

  // view cvec->norm as fvec->data
  fvec_t tmp;
  tmp.length = in->length;
  tmp.data = in->norm;

  if (f->power != 1.) fvec_pow(&tmp, f->power);

  fmat_vecmul(f->filters, &tmp, out);

  return;
}