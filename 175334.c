uint_t aubio_tempo_set_threshold(aubio_tempo_t * o, smpl_t threshold) {
  o->threshold = threshold;
  aubio_peakpicker_set_threshold(o->pp, o->threshold);
  return AUBIO_OK;
}