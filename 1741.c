float ActivationFunction(float x) {
  float output_activation_min, output_activation_max;
  GetActivationMinMax(Ac, &output_activation_min, &output_activation_max);
  return ActivationFunctionWithMinMax(x, output_activation_min,
                                      output_activation_max);
}