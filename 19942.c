PipelineD::buildInnerQueryExecutor(const CollectionPtr& collection,
                                   const NamespaceString& nss,
                                   const AggregateCommandRequest* aggRequest,
                                   Pipeline* pipeline) {
    auto expCtx = pipeline->getContext();

    // We will be modifying the source vector as we go.
    Pipeline::SourceContainer& sources = pipeline->_sources;

    if (!sources.empty() && !sources.front()->constraints().requiresInputDocSource) {
        return {};
    }

    if (!sources.empty()) {
        // Try to inspect if the DocumentSourceSample or a DocumentSourceInternalUnpackBucket stage
        // can be optimized for sampling backed by a storage engine supplied random cursor.
        auto&& [sampleStage, unpackBucketStage] = extractSampleUnpackBucket(sources);

        // Optimize an initial $sample stage if possible.
        if (collection && sampleStage) {
            auto [attachExecutorCallback, exec] =
                buildInnerQueryExecutorSample(sampleStage, unpackBucketStage, collection, pipeline);
            if (exec) {
                return std::make_pair(std::move(attachExecutorCallback), std::move(exec));
            }
        }
    }

    // If the first stage is $geoNear, prepare a special DocumentSourceGeoNearCursor stage;
    // otherwise, create a generic DocumentSourceCursor.
    const auto geoNearStage =
        sources.empty() ? nullptr : dynamic_cast<DocumentSourceGeoNear*>(sources.front().get());
    if (geoNearStage) {
        return buildInnerQueryExecutorGeoNear(collection, nss, aggRequest, pipeline);
    } else {
        return buildInnerQueryExecutorGeneric(collection, nss, aggRequest, pipeline);
    }
}