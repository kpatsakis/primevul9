gimp_layer_mode_from_psp_blend_mode (PSPBlendModes mode)
{
  switch (mode)
    {
    case PSP_BLEND_NORMAL:
      return GIMP_NORMAL_MODE;
    case PSP_BLEND_DARKEN:
      return GIMP_DARKEN_ONLY_MODE;
    case PSP_BLEND_LIGHTEN:
      return GIMP_LIGHTEN_ONLY_MODE;
    case PSP_BLEND_HUE:
      return GIMP_HUE_MODE;
    case PSP_BLEND_SATURATION:
      return GIMP_SATURATION_MODE;
    case PSP_BLEND_COLOR:
      return GIMP_COLOR_MODE;
    case PSP_BLEND_LUMINOSITY:
      return GIMP_VALUE_MODE;   /* ??? */
    case PSP_BLEND_MULTIPLY:
      return GIMP_MULTIPLY_MODE;
    case PSP_BLEND_SCREEN:
      return GIMP_SCREEN_MODE;
    case PSP_BLEND_DISSOLVE:
      return GIMP_DISSOLVE_MODE;
    case PSP_BLEND_OVERLAY:
      return GIMP_OVERLAY_MODE;
    case PSP_BLEND_HARD_LIGHT:
      return GIMP_HARDLIGHT_MODE;
    case PSP_BLEND_SOFT_LIGHT:
      return GIMP_SOFTLIGHT_MODE;
    case PSP_BLEND_DIFFERENCE:
      return GIMP_DIFFERENCE_MODE;
    case PSP_BLEND_DODGE:
      return GIMP_DODGE_MODE;
    case PSP_BLEND_BURN:
      return GIMP_BURN_MODE;
    case PSP_BLEND_EXCLUSION:
      return -1;                /* ??? */
    case PSP_BLEND_ADJUST:
      return -1;                /* ??? */
    case PSP_BLEND_TRUE_HUE:
      return -1;                /* ??? */
    case PSP_BLEND_TRUE_SATURATION:
      return -1;                /* ??? */
    case PSP_BLEND_TRUE_COLOR:
      return -1;                /* ??? */
    case PSP_BLEND_TRUE_LIGHTNESS:
      return -1;                /* ??? */
    }
  return -1;
}