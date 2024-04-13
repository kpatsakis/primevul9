static bool not_same_qid(const V9fsQID *qid1, const V9fsQID *qid2)
{
    return
        qid1->type != qid2->type ||
        qid1->version != qid2->version ||
        qid1->path != qid2->path;
}