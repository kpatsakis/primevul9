mrb_ary_shift(mrb_state *mrb, mrb_value self)
{
  struct RArray *a = mrb_ary_ptr(self);
  mrb_int len = ARY_LEN(a);
  mrb_value val;

  ary_modify_check(mrb, a);
  if (len == 0) return mrb_nil_value();
  if (ARY_SHARED_P(a)) {
  L_SHIFT:
    val = a->as.heap.ptr[0];
    a->as.heap.ptr++;
    a->as.heap.len--;
    return val;
  }
  if (len > ARY_SHIFT_SHARED_MIN) {
    ary_make_shared(mrb, a);
    goto L_SHIFT;
  }
  else {
    mrb_value *ptr = ARY_PTR(a);
    mrb_int size = len;

    val = *ptr;
    while (--size) {
      *ptr = *(ptr+1);
      ++ptr;
    }
    ARY_SET_LEN(a, len-1);
  }
  return val;
}