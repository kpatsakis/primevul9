static inline int doLookChar(JPXStreamPrivate* priv) {
  if (unlikely(priv->counter >= priv->npixels))
    return EOF;

  return ((unsigned char *)priv->image->comps[priv->ccounter].data)[priv->counter];
}