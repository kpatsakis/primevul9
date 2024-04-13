static VALUE convert_encoding(VALUE source)
{
#ifdef HAVE_RUBY_ENCODING_H
  rb_encoding *enc = rb_enc_get(source);
  if (enc == rb_ascii8bit_encoding()) {
    if (OBJ_FROZEN(source)) {
      source = rb_str_dup(source);
    }
    FORCE_UTF8(source);
  } else {
    source = rb_str_conv_enc(source, rb_enc_get(source), rb_utf8_encoding());
  }
#endif
    return source;
}