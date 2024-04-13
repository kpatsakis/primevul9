_hb_ot_get_font_funcs (void)
{
  static const hb_font_funcs_t ot_ffuncs = {
    HB_OBJECT_HEADER_STATIC,

    true, /* immutable */

    {
#define HB_FONT_FUNC_IMPLEMENT(name) hb_ot_get_##name,
      HB_FONT_FUNCS_IMPLEMENT_CALLBACKS
#undef HB_FONT_FUNC_IMPLEMENT
    }
  };

  return const_cast<hb_font_funcs_t *> (&ot_ffuncs);
}