void sdma_dumpstate(struct sdma_engine *sde)
{
	u64 csr;
	unsigned i;

	sdma_dumpstate_helper(SD(CTRL));
	sdma_dumpstate_helper(SD(STATUS));
	sdma_dumpstate_helper0(SD(ERR_STATUS));
	sdma_dumpstate_helper0(SD(ERR_MASK));
	sdma_dumpstate_helper(SD(ENG_ERR_STATUS));
	sdma_dumpstate_helper(SD(ENG_ERR_MASK));

	for (i = 0; i < CCE_NUM_INT_CSRS; ++i) {
		sdma_dumpstate_helper2(CCE_INT_STATUS);
		sdma_dumpstate_helper2(CCE_INT_MASK);
		sdma_dumpstate_helper2(CCE_INT_BLOCKED);
	}

	sdma_dumpstate_helper(SD(TAIL));
	sdma_dumpstate_helper(SD(HEAD));
	sdma_dumpstate_helper(SD(PRIORITY_THLD));
	sdma_dumpstate_helper(SD(IDLE_CNT));
	sdma_dumpstate_helper(SD(RELOAD_CNT));
	sdma_dumpstate_helper(SD(DESC_CNT));
	sdma_dumpstate_helper(SD(DESC_FETCHED_CNT));
	sdma_dumpstate_helper(SD(MEMORY));
	sdma_dumpstate_helper0(SD(ENGINES));
	sdma_dumpstate_helper0(SD(MEM_SIZE));
	/* sdma_dumpstate_helper(SEND_EGRESS_SEND_DMA_STATUS);  */
	sdma_dumpstate_helper(SD(BASE_ADDR));
	sdma_dumpstate_helper(SD(LEN_GEN));
	sdma_dumpstate_helper(SD(HEAD_ADDR));
	sdma_dumpstate_helper(SD(CHECK_ENABLE));
	sdma_dumpstate_helper(SD(CHECK_VL));
	sdma_dumpstate_helper(SD(CHECK_JOB_KEY));
	sdma_dumpstate_helper(SD(CHECK_PARTITION_KEY));
	sdma_dumpstate_helper(SD(CHECK_SLID));
	sdma_dumpstate_helper(SD(CHECK_OPCODE));
}