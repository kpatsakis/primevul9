StatusWith<std::unique_ptr<PlanExecutor, PlanExecutor::Deleter>> PipelineD::prepareExecutor(
    const intrusive_ptr<ExpressionContext>& expCtx,
    const CollectionPtr& collection,
    const NamespaceString& nss,
    Pipeline* pipeline,
    const boost::intrusive_ptr<DocumentSourceSort>& sortStage,
    std::unique_ptr<GroupFromFirstDocumentTransformation> rewrittenGroupStage,
    QueryMetadataBitSet unavailableMetadata,
    const BSONObj& queryObj,
    SkipThenLimit skipThenLimit,
    const AggregateCommandRequest* aggRequest,
    const MatchExpressionParser::AllowedFeatureSet& matcherFeatures,
    bool* hasNoRequirements) {
    invariant(hasNoRequirements);

    // Any data returned from the inner executor must be owned.
    size_t plannerOpts = QueryPlannerParams::DEFAULT;

    if (pipeline->peekFront() && pipeline->peekFront()->constraints().isChangeStreamStage()) {
        invariant(expCtx->tailableMode == TailableModeEnum::kTailableAndAwaitData);
        plannerOpts |= (QueryPlannerParams::TRACK_LATEST_OPLOG_TS |
                        QueryPlannerParams::ASSERT_MIN_TS_HAS_NOT_FALLEN_OFF_OPLOG);
    }

    // The $_requestReshardingResumeToken parameter is only valid for an oplog scan.
    if (aggRequest && aggRequest->getRequestReshardingResumeToken()) {
        plannerOpts |= (QueryPlannerParams::TRACK_LATEST_OPLOG_TS |
                        QueryPlannerParams::ASSERT_MIN_TS_HAS_NOT_FALLEN_OFF_OPLOG);
    }

    // If there is a sort stage eligible for pushdown, serialize its SortPattern to a BSONObj. The
    // BSONObj format is currently necessary to request that the sort is computed by the query layer
    // inside the inner PlanExecutor. We also remove the $sort stage from the Pipeline, since it
    // will be handled instead by PlanStage execution.
    BSONObj sortObj;
    if (sortStage) {
        sortObj = sortStage->getSortKeyPattern()
                      .serialize(SortPattern::SortKeySerialization::kForPipelineSerialization)
                      .toBson();

        pipeline->popFrontWithName(DocumentSourceSort::kStageName);

        // Now that we've pushed down the sort, see if there is a $limit and $skip to push down
        // also. We should not already have a limit or skip here, otherwise it would be incorrect
        // for the caller to pass us a sort stage to push down, since the order matters.
        invariant(!skipThenLimit.getLimit());
        invariant(!skipThenLimit.getSkip());

        // Since all $limit stages were already pushdowned to the sort stage, we are only looking
        // for $skip stages.
        auto skip = extractSkipForPushdown(pipeline);

        // Since the limit from $sort is going before the extracted $skip stages, we construct
        // 'LimitThenSkip' object and then convert it 'SkipThenLimit'.
        skipThenLimit = LimitThenSkip(sortStage->getLimit(), skip).flip();
    }

    // Perform dependency analysis. In order to minimize the dependency set, we only analyze the
    // stages that remain in the pipeline after pushdown. In particular, any dependencies for a
    // $match or $sort pushed down into the query layer will not be reflected here.
    auto deps = pipeline->getDependencies(unavailableMetadata);
    *hasNoRequirements = deps.hasNoRequirements();

    BSONObj projObj;
    if (*hasNoRequirements) {
        // This query might be eligible for count optimizations, since the remaining stages in the
        // pipeline don't actually need to read any data produced by the query execution layer.
        plannerOpts |= QueryPlannerParams::IS_COUNT;
    } else {
        // Build a BSONObj representing a projection eligible for pushdown. If there is an inclusion
        // projection at the front of the pipeline, it will be removed and handled by the PlanStage
        // layer. If a projection cannot be pushed down, an empty BSONObj will be returned.

        // In most cases .find() behaves as if it evaluates in a predictable order:
        //     predicate, sort, skip, limit, projection.
        // But there is at least one case where it runs the projection before the sort/skip/limit:
        // when the predicate has a rooted $or.  (In that case we plan each branch of the $or
        // separately, using Subplan, and include the projection on each branch.)

        // To work around this behavior, don't allow pushing down expressions if we are also going
        // to push down a sort, skip or limit. We don't want the expressions to be evaluated on any
        // documents that the sort/skip/limit would have filtered out. (The sort stage can be a
        // top-k sort, which both sorts and limits.)
        bool allowExpressions = !sortStage && !skipThenLimit.getSkip() && !skipThenLimit.getLimit();
        projObj = buildProjectionForPushdown(deps, pipeline, allowExpressions);
        plannerOpts |= QueryPlannerParams::RETURN_OWNED_DATA;
    }

    if (rewrittenGroupStage) {
        // See if the query system can handle the $group and $sort stage using a DISTINCT_SCAN
        // (SERVER-9507).
        auto swExecutorGrouped = attemptToGetExecutor(expCtx,
                                                      collection,
                                                      nss,
                                                      queryObj,
                                                      projObj,
                                                      deps.metadataDeps(),
                                                      sortObj,
                                                      SkipThenLimit{boost::none, boost::none},
                                                      rewrittenGroupStage->groupId(),
                                                      aggRequest,
                                                      plannerOpts,
                                                      matcherFeatures);

        if (swExecutorGrouped.isOK()) {
            // Any $limit stage before the $group stage should make the pipeline ineligible for this
            // optimization.
            invariant(!sortStage || !sortStage->hasLimit());

            // We remove the $sort and $group stages that begin the pipeline, because the executor
            // will handle the sort, and the groupTransform (added below) will handle the $group
            // stage.
            pipeline->popFrontWithName(DocumentSourceSort::kStageName);
            pipeline->popFrontWithName(DocumentSourceGroup::kStageName);

            boost::intrusive_ptr<DocumentSource> groupTransform(
                new DocumentSourceSingleDocumentTransformation(
                    expCtx,
                    std::move(rewrittenGroupStage),
                    "$groupByDistinctScan",
                    false /* independentOfAnyCollection */));
            pipeline->addInitialSource(groupTransform);

            return swExecutorGrouped;
        } else if (swExecutorGrouped != ErrorCodes::NoQueryExecutionPlans) {
            return swExecutorGrouped.getStatus().withContext(
                "Failed to determine whether query system can provide a "
                "DISTINCT_SCAN grouping");
        }
    }

    return attemptToGetExecutor(expCtx,
                                collection,
                                nss,
                                queryObj,
                                projObj,
                                deps.metadataDeps(),
                                sortObj,
                                skipThenLimit,
                                boost::none, /* groupIdForDistinctScan */
                                aggRequest,
                                plannerOpts,
                                matcherFeatures);
}