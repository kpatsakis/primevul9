  inline void fini (void)
  {
    hb_blob_destroy (this->loca_blob);
    hb_blob_destroy (this->glyf_blob);
  }