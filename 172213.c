int JPXStream::lookChar() {
  if (unlikely(priv->inited == false)) { init(); }

  return doLookChar(priv);
}