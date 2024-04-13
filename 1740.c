inline void gen_lut(FloatT (*func)(FloatT), FloatT input_min, FloatT input_max,
                    FloatT output_min, FloatT output_max, LutOutT* lut) {
  static_assert(std::is_same<LutInT, int8_t>::value ||
                    std::is_same<LutInT, int16_t>::value,
                "Only LUTs with int8 or int16 inputs are supported.");
  static_assert(std::is_same<LutOutT, int8_t>::value ||
                    std::is_same<LutOutT, int16_t>::value,
                "Only LUTs with int8 or int16 outputs are supported.");
  static_assert(std::is_floating_point<FloatT>::value,
                "FloatT must be a floating-point type.");

  const int nb_steps = std::is_same<LutInT, int8_t>::value ? 256 : 512;
  const FloatT step = (input_max - input_min) / nb_steps;
  const FloatT half_step = step / 2;
  const FloatT output_scaling_inv =
      static_cast<FloatT>(std::numeric_limits<LutOutT>::max() -
                          std::numeric_limits<LutOutT>::min() + 1) /
      (output_max - output_min);
  const FloatT table_min =
      static_cast<FloatT>(std::numeric_limits<LutOutT>::min());
  const FloatT table_max =
      static_cast<FloatT>(std::numeric_limits<LutOutT>::max());

  for (int i = 0; i < nb_steps; i++) {
    const FloatT val = func(input_min + i * step);
    const FloatT val_midpoint = func(input_min + i * step + half_step);
    const FloatT val_next = func(input_min + (i + 1) * step);

    const FloatT sample_val = TfLiteRound(val * output_scaling_inv);
    const FloatT midpoint_interp_val =
        TfLiteRound((val_next * output_scaling_inv +
                     TfLiteRound(val * output_scaling_inv)) /
                    2);
    const FloatT midpoint_val = TfLiteRound(val_midpoint * output_scaling_inv);
    const FloatT midpoint_err = midpoint_interp_val - midpoint_val;
    const FloatT bias = TfLiteRound(midpoint_err / 2);

    lut[i] = static_cast<LutOutT>(std::min<FloatT>(
        std::max<FloatT>(sample_val - bias, table_min), table_max));
  }

  const bool with_extra_interpolation_value =
      std::is_same<LutInT, int16_t>::value;
  if (with_extra_interpolation_value) {
    lut[nb_steps] = static_cast<LutOutT>(std::min<FloatT>(
        std::max<FloatT>(TfLiteRound(func(input_max) * output_scaling_inv),
                         table_min),
        table_max));
  }
}