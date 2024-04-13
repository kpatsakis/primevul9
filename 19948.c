PipelineD::buildInnerQueryExecutorGeneric(const CollectionPtr& collection,
                                          const NamespaceString& nss,
                                          const AggregateCommandRequest* aggRequest,
                                          Pipeline* pipeline) {
    // Make a last effort to optimize pipeline stages before potentially detaching them to be pushed
    // down into the query executor.
    pipeline->optimizePipeline();

    Pipeline::SourceContainer& sources = pipeline->_sources;
    auto expCtx = pipeline->getContext();

    // Look for an initial match. This works whether we got an initial query or not. If not, it
    // results in a "{}" query, which will be what we want in that case.
    const BSONObj queryObj = pipeline->getInitialQuery();
    if (!queryObj.isEmpty()) {
        auto matchStage = dynamic_cast<DocumentSourceMatch*>(sources.front().get());
        if (matchStage) {
            // If a $match query is pulled into the cursor, the $match is redundant, and can be
            // removed from the pipeline.
            sources.pop_front();
        } else {
            // A $geoNear stage, the only other stage that can produce an initial query, is also
            // a valid initial stage. However, we should be in prepareGeoNearCursorSource() instead.
            MONGO_UNREACHABLE;
        }
    }

    auto&& [sortStage, groupStage] = getSortAndGroupStagesFromPipeline(pipeline->_sources);
    std::unique_ptr<GroupFromFirstDocumentTransformation> rewrittenGroupStage;
    if (groupStage) {
        rewrittenGroupStage = groupStage->rewriteGroupAsTransformOnFirstDocument();
    }

    // If there is a $limit or $skip stage (or multiple of them) that could be pushed down into the
    // PlanStage layer, obtain the value of the limit and skip and remove the $limit and $skip
    // stages from the pipeline.
    //
    // This analysis is done here rather than in 'optimizePipeline()' because swapping $limit before
    // stages such as $project is not always useful, and can sometimes defeat other optimizations.
    // In particular, in a sharded scenario a pipeline such as [$project, $limit] is preferable to
    // [$limit, $project]. The former permits the execution of the projection operation to be
    // parallelized across all targeted shards, whereas the latter would bring all of the data to a
    // merging shard first, and then apply the projection serially. See SERVER-24981 for a more
    // detailed discussion.
    //
    // This only handles the case in which the the $limit or $skip can logically be swapped to the
    // front of the pipeline. We can also push down a $limit which comes after a $sort into the
    // PlanStage layer, but that is handled elsewhere.
    const auto skipThenLimit = extractSkipAndLimitForPushdown(pipeline);

    auto unavailableMetadata = DocumentSourceMatch::isTextQuery(queryObj)
        ? DepsTracker::kDefaultUnavailableMetadata & ~DepsTracker::kOnlyTextScore
        : DepsTracker::kDefaultUnavailableMetadata;

    // Create the PlanExecutor.
    bool shouldProduceEmptyDocs = false;
    auto exec = uassertStatusOK(prepareExecutor(expCtx,
                                                collection,
                                                nss,
                                                pipeline,
                                                sortStage,
                                                std::move(rewrittenGroupStage),
                                                unavailableMetadata,
                                                queryObj,
                                                skipThenLimit,
                                                aggRequest,
                                                Pipeline::kAllowedMatcherFeatures,
                                                &shouldProduceEmptyDocs));

    const auto cursorType = shouldProduceEmptyDocs
        ? DocumentSourceCursor::CursorType::kEmptyDocuments
        : DocumentSourceCursor::CursorType::kRegular;

    // If this is a change stream pipeline or a resharding resume token has been requested, make
    // sure that we tell DSCursor to track the oplog time.
    const bool trackOplogTS =
        (pipeline->peekFront() && pipeline->peekFront()->constraints().isChangeStreamStage()) ||
        (aggRequest && aggRequest->getRequestReshardingResumeToken());

    auto attachExecutorCallback =
        [cursorType, trackOplogTS](const CollectionPtr& collection,
                                   std::unique_ptr<PlanExecutor, PlanExecutor::Deleter> exec,
                                   Pipeline* pipeline) {
            auto cursor = DocumentSourceCursor::create(
                collection, std::move(exec), pipeline->getContext(), cursorType, trackOplogTS);
            pipeline->addInitialSource(std::move(cursor));
        };
    return std::make_pair(std::move(attachExecutorCallback), std::move(exec));
}