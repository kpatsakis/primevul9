PipelineD::buildInnerQueryExecutorGeoNear(const CollectionPtr& collection,
                                          const NamespaceString& nss,
                                          const AggregateCommandRequest* aggRequest,
                                          Pipeline* pipeline) {
    uassert(ErrorCodes::NamespaceNotFound,
            str::stream() << "$geoNear requires a geo index to run, but " << nss.ns()
                          << " does not exist",
            collection);

    Pipeline::SourceContainer& sources = pipeline->_sources;
    auto expCtx = pipeline->getContext();
    const auto geoNearStage = dynamic_cast<DocumentSourceGeoNear*>(sources.front().get());
    invariant(geoNearStage);

    // If the user specified a "key" field, use that field to satisfy the "near" query. Otherwise,
    // look for a geo-indexed field in 'collection' that can.
    auto nearFieldName =
        (geoNearStage->getKeyField() ? geoNearStage->getKeyField()->fullPath()
                                     : extractGeoNearFieldFromIndexes(expCtx->opCtx, collection))
            .toString();

    // Create a PlanExecutor whose query is the "near" predicate on 'nearFieldName' combined with
    // the optional "query" argument in the $geoNear stage.
    BSONObj fullQuery = geoNearStage->asNearQuery(nearFieldName);

    bool shouldProduceEmptyDocs = false;
    auto exec = uassertStatusOK(
        prepareExecutor(expCtx,
                        collection,
                        nss,
                        pipeline,
                        nullptr, /* sortStage */
                        nullptr, /* rewrittenGroupStage */
                        DepsTracker::kDefaultUnavailableMetadata & ~DepsTracker::kAllGeoNearData,
                        std::move(fullQuery),
                        SkipThenLimit{boost::none, boost::none},
                        aggRequest,
                        Pipeline::kGeoNearMatcherFeatures,
                        &shouldProduceEmptyDocs));

    auto attachExecutorCallback = [distanceField = geoNearStage->getDistanceField(),
                                   locationField = geoNearStage->getLocationField(),
                                   distanceMultiplier =
                                       geoNearStage->getDistanceMultiplier().value_or(1.0)](
                                      const CollectionPtr& collection,
                                      std::unique_ptr<PlanExecutor, PlanExecutor::Deleter> exec,
                                      Pipeline* pipeline) {
        auto cursor = DocumentSourceGeoNearCursor::create(collection,
                                                          std::move(exec),
                                                          pipeline->getContext(),
                                                          distanceField,
                                                          locationField,
                                                          distanceMultiplier);
        pipeline->addInitialSource(std::move(cursor));
    };
    // Remove the initial $geoNear; it will be replaced by $geoNearCursor.
    sources.pop_front();
    return std::make_pair(std::move(attachExecutorCallback), std::move(exec));
}