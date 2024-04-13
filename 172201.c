int JPXStream::getChars(int nChars, unsigned char *buffer) {
  if (unlikely(priv->inited == false)) { init(); }

  for (int i = 0; i < nChars; ++i) {
    const int c = doGetChar(priv);
    if (likely(c != EOF)) buffer[i] = c;
    else return i;
  }
  return nChars;
}