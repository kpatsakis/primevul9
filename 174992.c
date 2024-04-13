Error Box_dref::parse(BitstreamRange& range)
{
  parse_full_box_header(range);

  int nEntities = range.read32();

  /*
  for (int i=0;i<nEntities;i++) {
    if (range.eof()) {
      break;
    }
  }
  */

  Error err = read_children(range, nEntities);
  if (err) {
    return err;
  }

  if ((int)m_children.size() != nEntities) {
    // TODO return Error(
  }

  return err;
}