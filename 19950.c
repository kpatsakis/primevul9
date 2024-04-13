StatusWith<std::unique_ptr<PlanExecutor, PlanExecutor::Deleter>> attemptToGetExecutor(
    const intrusive_ptr<ExpressionContext>& expCtx,
    const CollectionPtr& collection,
    const NamespaceString& nss,
    BSONObj queryObj,
    BSONObj projectionObj,
    const QueryMetadataBitSet& metadataRequested,
    BSONObj sortObj,
    SkipThenLimit skipThenLimit,
    boost::optional<std::string> groupIdForDistinctScan,
    const AggregateCommandRequest* aggRequest,
    const size_t plannerOpts,
    const MatchExpressionParser::AllowedFeatureSet& matcherFeatures) {
    auto findCommand = std::make_unique<FindCommandRequest>(nss);
    query_request_helper::setTailableMode(expCtx->tailableMode, findCommand.get());
    findCommand->setFilter(queryObj.getOwned());
    findCommand->setProjection(projectionObj.getOwned());
    findCommand->setSort(sortObj.getOwned());
    if (auto skip = skipThenLimit.getSkip()) {
        findCommand->setSkip(static_cast<std::int64_t>(*skip));
    }
    if (auto limit = skipThenLimit.getLimit()) {
        findCommand->setLimit(static_cast<std::int64_t>(*limit));
    }

    bool isExplain = false;
    if (aggRequest) {
        findCommand->setHint(aggRequest->getHint().value_or(BSONObj()).getOwned());
        isExplain = static_cast<bool>(aggRequest->getExplain());
    }

    // The collation on the ExpressionContext has been resolved to either the user-specified
    // collation or the collection default. This BSON should never be empty even if the resolved
    // collator is simple.
    findCommand->setCollation(expCtx->getCollatorBSON().getOwned());

    const ExtensionsCallbackReal extensionsCallback(expCtx->opCtx, &nss);

    // Reset the 'sbeCompatible' flag before canonicalizing the 'findCommand' to potentially allow
    // SBE to execute the portion of the query that's pushed down, even if the portion of the query
    // that is not pushed down contains expressions not supported by SBE.
    expCtx->sbeCompatible = true;

    auto cq = CanonicalQuery::canonicalize(expCtx->opCtx,
                                           std::move(findCommand),
                                           isExplain,
                                           expCtx,
                                           extensionsCallback,
                                           matcherFeatures,
                                           ProjectionPolicies::aggregateProjectionPolicies());

    if (!cq.isOK()) {
        // Return an error instead of uasserting, since there are cases where the combination of
        // sort and projection will result in a bad query, but when we try with a different
        // combination it will be ok. e.g. a sort by {$meta: 'textScore'}, without any projection
        // will fail, but will succeed when the corresponding '$meta' projection is passed in
        // another attempt.
        return {cq.getStatus()};
    }

    // Mark the metadata that's requested by the pipeline on the CQ.
    cq.getValue()->requestAdditionalMetadata(metadataRequested);

    if (groupIdForDistinctScan) {
        // When the pipeline includes a $group that groups by a single field
        // (groupIdForDistinctScan), we use getExecutorDistinct() to attempt to get an executor that
        // uses a DISTINCT_SCAN to scan exactly one document for each group. When that's not
        // possible, we return nullptr, and the caller is responsible for trying again without
        // passing a 'groupIdForDistinctScan' value.
        ParsedDistinct parsedDistinct(std::move(cq.getValue()), *groupIdForDistinctScan);

        // Note that we request a "strict" distinct plan because:
        // 1) We do not want to have to de-duplicate the results of the plan.
        //
        // 2) We not want a plan that will return separate values for each array element. For
        // example, if we have a document {a: [1,2]} and group by "a" a DISTINCT_SCAN on an "a"
        // index would produce one result for '1' and another for '2', which would be incorrect.
        auto distinctExecutor = getExecutorDistinct(
            &collection, plannerOpts | QueryPlannerParams::STRICT_DISTINCT_ONLY, &parsedDistinct);
        if (!distinctExecutor.isOK()) {
            return distinctExecutor.getStatus().withContext(
                "Unable to use distinct scan to optimize $group stage");
        } else if (!distinctExecutor.getValue()) {
            return {ErrorCodes::NoQueryExecutionPlans,
                    "Unable to use distinct scan to optimize $group stage"};
        } else {
            return distinctExecutor;
        }
    }

    bool permitYield = true;
    return getExecutorFind(
        expCtx->opCtx, &collection, std::move(cq.getValue()), permitYield, plannerOpts);
}