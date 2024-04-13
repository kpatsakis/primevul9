static void svcxdr_init_encode(struct svc_rqst *rqstp,
			       struct nfsd4_compoundres *resp)
{
	struct xdr_stream *xdr = &resp->xdr;
	struct xdr_buf *buf = &rqstp->rq_res;
	struct kvec *head = buf->head;

	xdr->buf = buf;
	xdr->iov = head;
	xdr->p   = head->iov_base + head->iov_len;
	xdr->end = head->iov_base + PAGE_SIZE - rqstp->rq_auth_slack;
	/* Tail and page_len should be zero at this point: */
	buf->len = buf->head[0].iov_len;
	xdr->scratch.iov_len = 0;
	xdr->page_ptr = buf->pages - 1;
	buf->buflen = PAGE_SIZE * (1 + rqstp->rq_page_end - buf->pages)
		- rqstp->rq_auth_slack;
}