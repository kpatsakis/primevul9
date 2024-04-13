uint_t aubio_tempo_set_delay_s(aubio_tempo_t * o, smpl_t delay) {
  o->delay = delay * o->samplerate;
  return AUBIO_OK;
}