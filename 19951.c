auto buildProjectionForPushdown(const DepsTracker& deps,
                                Pipeline* pipeline,
                                bool allowExpressions) {
    auto&& sources = pipeline->getSources();

    // Short-circuit if the pipeline is empty: there is no projection and nothing to push down.
    if (sources.empty()) {
        return BSONObj();
    }

    if (const auto projStage =
            exact_pointer_cast<DocumentSourceSingleDocumentTransformation*>(sources.front().get());
        projStage) {
        if (projStage->getType() == TransformerInterface::TransformerType::kInclusionProjection) {
            auto projObj =
                projStage->getTransformer().serializeTransformation(boost::none).toBson();
            auto projAst = projection_ast::parse(projStage->getContext(),
                                                 projObj,
                                                 ProjectionPolicies::aggregateProjectionPolicies());
            if (!projAst.hasExpressions() || allowExpressions) {
                // If there is an inclusion projection at the front of the pipeline, we have case 1.
                sources.pop_front();
                return projObj;
            }
        }
    }

    // Depending of whether there is a finite dependency set, either return a projection
    // representing this dependency set, or an empty BSON, meaning no projection push down will
    // happen. This covers cases 2 and 3.
    if (deps.getNeedsAnyMetadata())
        return BSONObj();
    return deps.toProjectionWithoutMetadata();
}