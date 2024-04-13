smpl_t aubio_tempo_get_delay_s(aubio_tempo_t * o) {
  return o->delay / (smpl_t)(o->samplerate);
}