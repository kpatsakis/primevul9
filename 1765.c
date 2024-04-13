inline std::int32_t SaturatingSub(std::int32_t a, std::int32_t b) {
  std::int64_t a64 = a;
  std::int64_t b64 = b;
  std::int64_t diff = a64 - b64;
  return static_cast<std::int32_t>(std::min(
      static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max()),
      std::max(
          static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::min()),
          diff)));
}