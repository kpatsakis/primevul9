  inline bool get_glyph (hb_codepoint_t  unicode,
			 hb_codepoint_t  variation_selector,
			 hb_codepoint_t *glyph) const
  {
    if (unlikely (variation_selector))
    {
      switch (this->uvs_table->get_glyph_variant (unicode,
						  variation_selector,
						  glyph))
      {
	case OT::GLYPH_VARIANT_NOT_FOUND:	return false;
	case OT::GLYPH_VARIANT_FOUND:		return true;
	case OT::GLYPH_VARIANT_USE_DEFAULT:	break;
      }
    }

    return this->table->get_glyph (unicode, glyph);
  }