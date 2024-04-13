void PipelineD::attachInnerQueryExecutorToPipeline(
    const CollectionPtr& collection,
    PipelineD::AttachExecutorCallback attachExecutorCallback,
    std::unique_ptr<PlanExecutor, PlanExecutor::Deleter> exec,
    Pipeline* pipeline) {
    // If the pipeline doesn't need a $cursor stage, there will be no callback function and
    // PlanExecutor provided in the 'attachExecutorCallback' object, so we don't need to do
    // anything.
    if (attachExecutorCallback && exec) {
        attachExecutorCallback(collection, std::move(exec), pipeline);
    }
}