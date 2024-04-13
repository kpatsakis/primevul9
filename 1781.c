constexpr int min_log_x_output_bits(int input_bits) {
  return input_bits > 90   ? 7
         : input_bits > 44 ? 6
         : input_bits > 21 ? 5
         : input_bits > 10 ? 4
         : input_bits > 4  ? 3
         : input_bits > 1  ? 2
                           : 1;
}