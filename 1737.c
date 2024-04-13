inline int LegacyHowManyThreads(int max_num_threads, int rows, int cols,
                                int depth) {
  // Early-exit in the default case where multi-threading is disabled.
  if (max_num_threads == 1) {
    return 1;
  }

  // Ensure that each thread has KernelRows rows to process, if at all possible.
  int thread_count = std::min(max_num_threads, rows / KernelRows);

  // Limit the number of threads according to the overall size of the problem.
  if (thread_count > 1) {
    // Empirically determined value.
    static constexpr std::uint64_t min_cubic_size_per_thread = 64 * 1024;

    // We can only multiply two out of three sizes without risking overflow
    const std::uint64_t cubic_size =
        std::uint64_t(rows) * std::uint64_t(cols) * std::uint64_t(depth);

    thread_count = std::min(
        thread_count, static_cast<int>(cubic_size / min_cubic_size_per_thread));
  }

  if (thread_count < 1) {
    thread_count = 1;
  }

  assert(thread_count > 0 && thread_count <= max_num_threads);
  return thread_count;
}