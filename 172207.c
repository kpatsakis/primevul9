void JPXStream::close() {
  if (priv->image != nullptr) {
    opj_image_destroy(priv->image);
    priv->image = nullptr;
    priv->npixels = 0;
  }
}