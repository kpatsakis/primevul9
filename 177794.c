dlmunmap (void *start, size_t length)
{
  /* We don't bother decreasing execsize or truncating the file, since
     we can't quite tell whether we're unmapping the end of the file.
     We don't expect frequent deallocation anyway.  If we did, we
     could locate pages in the file by writing to the pages being
     deallocated and checking that the file contents change.
     Yuck.  */
  msegmentptr seg = segment_holding (gm, start);
  void *code;

  if (seg && (code = add_segment_exec_offset (start, seg)) != start)
    {
      int ret = munmap (code, length);
      if (ret)
	return ret;
    }

  return munmap (start, length);
}