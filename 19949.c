boost::optional<long long> extractSkipForPushdown(Pipeline* pipeline) {
    // If the disablePipelineOptimization failpoint is enabled, then do not attempt the skip
    // pushdown optimization.
    if (MONGO_unlikely(disablePipelineOptimization.shouldFail())) {
        return boost::none;
    }
    auto&& sources = pipeline->getSources();

    auto skip = extractSkipForPushdown(sources.begin(), &sources);
    if (skip) {
        // Removing stages may have produced the opportunity for additional optimizations.
        pipeline->optimizePipeline();
    }
    return skip;
}