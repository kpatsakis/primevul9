inline T ActivationFunctionWithMinMax(T x, T output_activation_min,
                                      T output_activation_max) {
  using std::max;
  using std::min;
  return min(max(x, output_activation_min), output_activation_max);
}