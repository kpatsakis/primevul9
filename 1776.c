inline std::int16_t SaturatingSub(std::int16_t a, std::int16_t b) {
  std::int32_t a32 = a;
  std::int32_t b32 = b;
  std::int32_t diff = a32 - b32;
  return static_cast<std::int16_t>(
      std::min(static_cast<int32_t>(32767),
               std::max(static_cast<int32_t>(-32768), diff)));
}