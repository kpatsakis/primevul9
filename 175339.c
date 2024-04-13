uint_t aubio_tempo_set_delay(aubio_tempo_t * o, sint_t delay) {
  o->delay = delay;
  return AUBIO_OK;
}