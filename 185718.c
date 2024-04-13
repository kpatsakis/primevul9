  inline bool get_extents (hb_codepoint_t glyph,
			   hb_glyph_extents_t *extents) const
  {
    if (unlikely (glyph >= this->num_glyphs))
      return false;

    unsigned int start_offset, end_offset;
    if (this->short_offset)
    {
      start_offset = 2 * this->loca->u.shortsZ[glyph];
      end_offset   = 2 * this->loca->u.shortsZ[glyph + 1];
    }
    else
    {
      start_offset = this->loca->u.longsZ[glyph];
      end_offset   = this->loca->u.longsZ[glyph + 1];
    }

    if (start_offset > end_offset || end_offset > this->glyf_len)
      return false;

    if (end_offset - start_offset < OT::glyfGlyphHeader::static_size)
      return true; /* Empty glyph; zero extents. */

    const OT::glyfGlyphHeader &glyph_header = OT::StructAtOffset<OT::glyfGlyphHeader> (this->glyf, start_offset);

    extents->x_bearing = MIN (glyph_header.xMin, glyph_header.xMax);
    extents->y_bearing = MAX (glyph_header.yMin, glyph_header.yMax);
    extents->width     = MAX (glyph_header.xMin, glyph_header.xMax) - extents->x_bearing;
    extents->height    = MIN (glyph_header.yMin, glyph_header.yMax) - extents->y_bearing;

    return true;
  }