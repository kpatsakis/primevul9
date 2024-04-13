inline void GetActivationMinMax(FusedActivationFunctionType ac,
                                float* output_activation_min,
                                float* output_activation_max) {
  switch (ac) {
    case FusedActivationFunctionType::kNone:
      *output_activation_min = std::numeric_limits<float>::lowest();
      *output_activation_max = std::numeric_limits<float>::max();
      break;
    case FusedActivationFunctionType::kRelu:
      *output_activation_min = 0.f;
      *output_activation_max = std::numeric_limits<float>::max();
      break;
    case FusedActivationFunctionType::kRelu1:
      *output_activation_min = -1.f;
      *output_activation_max = 1.f;
      break;
    case FusedActivationFunctionType::kRelu6:
      *output_activation_min = 0.f;
      *output_activation_max = 6.f;
      break;
  }
}