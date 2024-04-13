uint_t aubio_tempo_set_delay_ms(aubio_tempo_t * o, smpl_t delay) {
  o->delay = 1000. * delay * o->samplerate;
  return AUBIO_OK;
}