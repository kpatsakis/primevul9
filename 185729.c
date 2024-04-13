  inline unsigned int get_advance (hb_codepoint_t glyph) const
  {
    if (unlikely (glyph >= this->num_metrics))
    {
      /* If this->num_metrics is zero, it means we don't have the metrics table
       * for this direction: return default advance.  Otherwise, it means that the
       * glyph index is out of bound: return zero. */
      if (this->num_metrics)
	return 0;
      else
	return this->default_advance;
    }

    if (glyph >= this->num_advances)
      glyph = this->num_advances - 1;

    return this->table->longMetric[glyph].advance;
  }