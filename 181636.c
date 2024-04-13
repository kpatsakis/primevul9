void KaxInternalBlock::ReleaseFrames()
{
  // free the allocated Frames
  int i;
  for (i=myBuffers.size()-1; i>=0; i--) {
    if (myBuffers[i] != NULL) {
      myBuffers[i]->FreeBuffer(*myBuffers[i]);
      delete myBuffers[i];
      myBuffers[i] = NULL;
    }
  }
}