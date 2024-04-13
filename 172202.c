static inline int doGetChar(JPXStreamPrivate* priv) {
  const int result = doLookChar(priv);
  if (++priv->ccounter == priv->ncomps) {
    priv->ccounter = 0;
    ++priv->counter;
  }
  return result;
}