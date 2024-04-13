get_current_reader (void)
{
  struct vreader_s *vr;

  /* We only support one reader for now.  */
  vr = &vreader_table[0];

  /* Initialize the vreader item if not yet done. */
  if (!vr->valid)
    {
      vr->slot = -1;
      vr->valid = 1;
    }

  /* Try to open the reader. */
  if (vr->slot == -1)
    {
      vr->slot = apdu_open_reader (opt.reader_port);

      /* If we still don't have a slot, we have no readers.
	 Invalidate for now until a reader is attached. */
      if (vr->slot == -1)
	{
	  vr->valid = 0;
	}
    }

  /* Return the vreader index or -1.  */
  return vr->valid ? 0 : -1;
}