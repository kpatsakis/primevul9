void PipelineD::buildAndAttachInnerQueryExecutorToPipeline(
    const CollectionPtr& collection,
    const NamespaceString& nss,
    const AggregateCommandRequest* aggRequest,
    Pipeline* pipeline) {

    auto callback = PipelineD::buildInnerQueryExecutor(collection, nss, aggRequest, pipeline);
    PipelineD::attachInnerQueryExecutorToPipeline(
        collection, callback.first, std::move(callback.second), pipeline);
}