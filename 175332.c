void aubio_tempo_do(aubio_tempo_t *o, const fvec_t * input, fvec_t * tempo)
{
  uint_t i;
  uint_t winlen = o->winlen;
  uint_t step   = o->step;
  fvec_t * thresholded;
  aubio_pvoc_do (o->pv, input, o->fftgrain);
  aubio_specdesc_do (o->od, o->fftgrain, o->of);
  /*if (usedoubled) {
    aubio_specdesc_do(o2,fftgrain, onset2);
    onset->data[0] *= onset2->data[0];
  }*/
  /* execute every overlap_size*step */
  if (o->blockpos == (signed)step -1 ) {
    /* check dfframe */
    aubio_beattracking_do(o->bt,o->dfframe,o->out);
    /* rotate dfframe */
    for (i = 0 ; i < winlen - step; i++ )
      o->dfframe->data[i] = o->dfframe->data[i+step];
    for (i = winlen - step ; i < winlen; i++ )
      o->dfframe->data[i] = 0.;
    o->blockpos = -1;
  }
  o->blockpos++;
  aubio_peakpicker_do (o->pp, o->of, o->onset);
  // store onset detection function in second sample of vector
  //tempo->data[1] = o->onset->data[0];
  thresholded = aubio_peakpicker_get_thresholded_input(o->pp);
  o->dfframe->data[winlen - step + o->blockpos] = thresholded->data[0];
  /* end of second level loop */
  tempo->data[0] = 0; /* reset tactus */
  //i=0;
  for (i = 1; i < o->out->data[0]; i++ ) {
    /* if current frame is a predicted tactus */
    if (o->blockpos == FLOOR(o->out->data[i])) {
      tempo->data[0] = o->out->data[i] - FLOOR(o->out->data[i]); /* set tactus */
      /* test for silence */
      if (aubio_silence_detection(input, o->silence)==1) {
        tempo->data[0] = 0; // unset beat if silent
      }
      o->last_beat = o->total_frames + (uint_t)ROUND(tempo->data[0] * o->hop_size);
      o->last_tatum = o->last_beat;
    }
  }
  o->total_frames += o->hop_size;
  return;
}