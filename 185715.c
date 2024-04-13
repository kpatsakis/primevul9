  inline void init (hb_face_t *face)
  {
    hb_blob_t *head_blob = OT::Sanitizer<OT::head>::sanitize (face->reference_table (HB_OT_TAG_head));
    const OT::head *head = OT::Sanitizer<OT::head>::lock_instance (head_blob);
    if ((unsigned int) head->indexToLocFormat > 1 || head->glyphDataFormat != 0)
    {
      /* Unknown format.  Leave num_glyphs=0, that takes care of disabling us. */
      hb_blob_destroy (head_blob);
      return;
    }
    this->short_offset = 0 == head->indexToLocFormat;
    hb_blob_destroy (head_blob);

    this->loca_blob = OT::Sanitizer<OT::loca>::sanitize (face->reference_table (HB_OT_TAG_loca));
    this->loca = OT::Sanitizer<OT::loca>::lock_instance (this->loca_blob);
    this->glyf_blob = OT::Sanitizer<OT::glyf>::sanitize (face->reference_table (HB_OT_TAG_glyf));
    this->glyf = OT::Sanitizer<OT::glyf>::lock_instance (this->glyf_blob);

    this->num_glyphs = MAX (1u, hb_blob_get_length (this->loca_blob) / (this->short_offset ? 2 : 4)) - 1;
    this->glyf_len = hb_blob_get_length (this->glyf_blob);
  }