int JPXStream::getChar() {
  if (unlikely(priv->inited == false)) { init(); }

  return doGetChar(priv);
}