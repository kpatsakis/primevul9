PipelineD::buildInnerQueryExecutorSample(DocumentSourceSample* sampleStage,
                                         DocumentSourceInternalUnpackBucket* unpackBucketStage,
                                         const CollectionPtr& collection,
                                         Pipeline* pipeline) {
    tassert(5422105, "sampleStage cannot be a nullptr", sampleStage);

    auto expCtx = pipeline->getContext();

    Pipeline::SourceContainer& sources = pipeline->_sources;

    const long long sampleSize = sampleStage->getSampleSize();
    const long long numRecords = collection->getRecordStore()->numRecords(expCtx->opCtx);

    boost::optional<BucketUnpacker> bucketUnpacker;
    if (unpackBucketStage) {
        bucketUnpacker = unpackBucketStage->bucketUnpacker();
    }
    auto&& [exec, isStorageOptimizedSample] = uassertStatusOK(createRandomCursorExecutor(
        collection, expCtx, sampleSize, numRecords, std::move(bucketUnpacker)));

    AttachExecutorCallback attachExecutorCallback;
    if (exec) {
        if (!unpackBucketStage) {
            if (isStorageOptimizedSample) {
                // Replace $sample stage with $sampleFromRandomCursor stage.
                pipeline->popFront();
                std::string idString = collection->ns().isOplog() ? "ts" : "_id";
                pipeline->addInitialSource(DocumentSourceSampleFromRandomCursor::create(
                    expCtx, sampleSize, idString, numRecords));
            }
        } else {
            if (isStorageOptimizedSample) {
                // If there are non-nullptrs for 'sampleStage' and 'unpackBucketStage', then
                // 'unpackBucketStage' is at the front of the pipeline immediately followed by a
                // 'sampleStage'. We need to use a TrialStage approach to handle a problem where
                // ARHASH sampling can fail due to small measurement counts. We can push sampling
                // and bucket unpacking down to the PlanStage layer and erase $_internalUnpackBucket
                // and $sample.
                sources.erase(sources.begin());
                sources.erase(sources.begin());
            } else {
                // The TrialStage chose the backup plan and we need to erase just the
                // $_internalUnpackBucket stage and leave $sample where it is.
                sources.erase(sources.begin());
            }
        }

        // The order in which we evaluate these arguments is significant. We'd like to be
        // sure that the DocumentSourceCursor is created _last_, because if we run into a
        // case where a DocumentSourceCursor has been created (yet hasn't been put into a
        // Pipeline) and an exception is thrown, an invariant will trigger in the
        // DocumentSourceCursor. This is a design flaw in DocumentSourceCursor.
        auto deps = pipeline->getDependencies(DepsTracker::kAllMetadata);
        const auto cursorType = deps.hasNoRequirements()
            ? DocumentSourceCursor::CursorType::kEmptyDocuments
            : DocumentSourceCursor::CursorType::kRegular;
        attachExecutorCallback =
            [cursorType](const CollectionPtr& collection,
                         std::unique_ptr<PlanExecutor, PlanExecutor::Deleter> exec,
                         Pipeline* pipeline) {
                auto cursor = DocumentSourceCursor::create(
                    collection, std::move(exec), pipeline->getContext(), cursorType);
                pipeline->addInitialSource(std::move(cursor));
            };
        return std::pair(std::move(attachExecutorCallback), std::move(exec));
    }
    return std::pair(std::move(attachExecutorCallback), nullptr);
}