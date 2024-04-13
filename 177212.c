static CompositeOperator GIMPBlendModeToCompositeOperator(
  size_t blendMode)
{
  switch ( blendMode )
  {
    case GIMP_NORMAL_MODE:       return(OverCompositeOp);
    case GIMP_DISSOLVE_MODE:     return(DissolveCompositeOp);
    case GIMP_MULTIPLY_MODE:     return(MultiplyCompositeOp);
    case GIMP_SCREEN_MODE:       return(ScreenCompositeOp);
    case GIMP_OVERLAY_MODE:      return(OverlayCompositeOp);
    case GIMP_DIFFERENCE_MODE:   return(DifferenceCompositeOp);
    case GIMP_ADDITION_MODE:     return(ModulusAddCompositeOp);
    case GIMP_SUBTRACT_MODE:     return(ModulusSubtractCompositeOp);
    case GIMP_DARKEN_ONLY_MODE:  return(DarkenCompositeOp);
    case GIMP_LIGHTEN_ONLY_MODE: return(LightenCompositeOp);
    case GIMP_HUE_MODE:          return(HueCompositeOp);
    case GIMP_SATURATION_MODE:   return(SaturateCompositeOp);
    case GIMP_COLOR_MODE:        return(ColorizeCompositeOp);
    case GIMP_DODGE_MODE:        return(ColorDodgeCompositeOp);
    case GIMP_BURN_MODE:         return(ColorBurnCompositeOp);
    case GIMP_HARDLIGHT_MODE:    return(HardLightCompositeOp);
    case GIMP_DIVIDE_MODE:       return(DivideDstCompositeOp);
    /* these are the ones we don't support...yet */
    case GIMP_BEHIND_MODE:       return(OverCompositeOp);
    case GIMP_VALUE_MODE:        return(OverCompositeOp);
    default:                     return(OverCompositeOp);
  }
}