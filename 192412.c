static int is_bigdecimal_class(VALUE obj)
{
  if (cBigDecimal == Qundef) {
    if (rb_const_defined(rb_cObject, i_BigDecimal)) {
      cBigDecimal = rb_const_get_at(rb_cObject, i_BigDecimal);
    }
    else {
      return 0;
    }
  }
  return obj == cBigDecimal;
}