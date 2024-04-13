void del_aubio_tempo (aubio_tempo_t *o)
{
  del_aubio_specdesc(o->od);
  del_aubio_beattracking(o->bt);
  del_aubio_peakpicker(o->pp);
  del_aubio_pvoc(o->pv);
  del_fvec(o->out);
  del_fvec(o->of);
  del_cvec(o->fftgrain);
  del_fvec(o->dfframe);
  del_fvec(o->onset);
  AUBIO_FREE(o);
  return;
}