TEST_F(QueryPlannerTest, TagAccordingToCacheFailsOnBadInput) {
    const NamespaceString nss("test.collection");

    auto qr = stdx::make_unique<QueryRequest>(nss);
    qr->setFilter(BSON("a" << 3));
    auto statusWithCQ = CanonicalQuery::canonicalize(opCtx.get(), std::move(qr));
    ASSERT_OK(statusWithCQ.getStatus());
    std::unique_ptr<CanonicalQuery> scopedCq = std::move(statusWithCQ.getValue());

    std::unique_ptr<PlanCacheIndexTree> indexTree(new PlanCacheIndexTree());
    indexTree->setIndexEntry(IndexEntry(BSON("a" << 1), "a_1"));

    std::map<IndexEntry::Identifier, size_t> indexMap;

    // Null filter.
    Status s = QueryPlanner::tagAccordingToCache(NULL, indexTree.get(), indexMap);
    ASSERT_NOT_OK(s);

    // Null indexTree.
    s = QueryPlanner::tagAccordingToCache(scopedCq->root(), NULL, indexMap);
    ASSERT_NOT_OK(s);

    // Index not found.
    s = QueryPlanner::tagAccordingToCache(scopedCq->root(), indexTree.get(), indexMap);
    ASSERT_NOT_OK(s);

    // Index found once added to the map.
    indexMap[IndexEntry::Identifier{"a_1"}] = 0;
    s = QueryPlanner::tagAccordingToCache(scopedCq->root(), indexTree.get(), indexMap);
    ASSERT_OK(s);

    // Regenerate canonical query in order to clear tags.
    auto newQR = stdx::make_unique<QueryRequest>(nss);
    newQR->setFilter(BSON("a" << 3));
    statusWithCQ = CanonicalQuery::canonicalize(opCtx.get(), std::move(newQR));
    ASSERT_OK(statusWithCQ.getStatus());
    scopedCq = std::move(statusWithCQ.getValue());

    // Mismatched tree topology.
    PlanCacheIndexTree* child = new PlanCacheIndexTree();
    child->setIndexEntry(IndexEntry(BSON("a" << 1)));
    indexTree->children.push_back(child);
    s = QueryPlanner::tagAccordingToCache(scopedCq->root(), indexTree.get(), indexMap);
    ASSERT_NOT_OK(s);
}