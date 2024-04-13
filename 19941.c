SkipThenLimit extractSkipAndLimitForPushdown(Pipeline* pipeline) {
    // If the disablePipelineOptimization failpoint is enabled, then do not attempt the limit and
    // skip pushdown optimization.
    if (MONGO_unlikely(disablePipelineOptimization.shouldFail())) {
        return {boost::none, boost::none};
    }
    auto&& sources = pipeline->getSources();

    // It is important to call 'extractLimitForPushdown' before 'extractSkipForPushdown'. Otherwise
    // there could be a situation when $limit stages in pipeline would prevent
    // 'extractSkipForPushdown' from extracting all $skip stages.
    auto limit = extractLimitForPushdown(sources.begin(), &sources);
    auto skip = extractSkipForPushdown(sources.begin(), &sources);
    auto skipThenLimit = LimitThenSkip(limit, skip).flip();
    if (skipThenLimit.getSkip() || skipThenLimit.getLimit()) {
        // Removing stages may have produced the opportunity for additional optimizations.
        pipeline->optimizePipeline();
    }
    return skipThenLimit;
}