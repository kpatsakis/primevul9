TEST_F(QueryPlannerTest, IntersectSubtreeAndPred) {
    params.options = QueryPlannerParams::NO_TABLE_SCAN | QueryPlannerParams::INDEX_INTERSECTION;
    addIndex(BSON("a" << 1));
    addIndex(BSON("b" << 1));
    addIndex(BSON("c" << 1));
    runQuery(fromjson("{a: 1, $or: [{b:1}, {c:1}]}"));

    // This (can be) rewritten to $or:[ {a:1, b:1}, {c:1, d:1}].  We don't look for the various
    // single $or solutions as that's tested elsewhere.  We look for the intersect solution,
    // where each AND inside of the root OR is an and_sorted.
    size_t matches = 0;
    matches += numSolutionMatches(
        "{fetch: {filter: {a:1,$or:[{b:1},{c:1}]}, node: {or: {nodes: ["
        "{andSorted: {nodes: ["
        "{ixscan: {filter: null, pattern: {'a':1}}},"
        "{ixscan: {filter: null, pattern: {'b':1}}}]}},"
        "{andSorted: {nodes: ["
        "{ixscan: {filter: null, pattern: {'a':1}}},"
        "{ixscan: {filter: null, pattern: {'c':1}}}]}}]}}}}");
    matches += numSolutionMatches(
        "{fetch: {filter: {a:1,$or:[{b:1},{c:1}]}, node: {andHash: {nodes:["
        "{or: {nodes: [{ixscan:{filter:null, pattern:{b:1}}},"
        "{ixscan:{filter:null, pattern:{c:1}}}]}},"
        "{ixscan:{filter: null, pattern:{a:1}}}]}}}}");
    ASSERT_GREATER_THAN_OR_EQUALS(matches, 1U);
}