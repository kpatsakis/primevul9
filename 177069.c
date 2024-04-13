void gfs2_rlist_alloc(struct gfs2_rgrp_list *rlist, unsigned int state)
{
	unsigned int x;

	rlist->rl_ghs = kcalloc(rlist->rl_rgrps, sizeof(struct gfs2_holder),
				GFP_NOFS | __GFP_NOFAIL);
	for (x = 0; x < rlist->rl_rgrps; x++)
		gfs2_holder_init(rlist->rl_rgd[x]->rd_gl,
				state, 0,
				&rlist->rl_ghs[x]);
}