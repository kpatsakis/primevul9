static void gic_compute_misr(GICState *s, int cpu)
{
    uint32_t value = 0;
    int vcpu = cpu + GIC_NCPU;

    int num_eoi, num_valid, num_pending;

    gic_extract_lr_info(s, cpu, &num_eoi, &num_valid, &num_pending);

    /* EOI */
    if (num_eoi) {
        value |= R_GICH_MISR_EOI_MASK;
    }

    /* U: true if only 0 or 1 LR entry is valid */
    if ((s->h_hcr[cpu] & R_GICH_HCR_UIE_MASK) && (num_valid < 2)) {
        value |= R_GICH_MISR_U_MASK;
    }

    /* LRENP: EOICount is not 0 */
    if ((s->h_hcr[cpu] & R_GICH_HCR_LRENPIE_MASK) &&
        ((s->h_hcr[cpu] & R_GICH_HCR_EOICount_MASK) != 0)) {
        value |= R_GICH_MISR_LRENP_MASK;
    }

    /* NP: no pending interrupts */
    if ((s->h_hcr[cpu] & R_GICH_HCR_NPIE_MASK) && (num_pending == 0)) {
        value |= R_GICH_MISR_NP_MASK;
    }

    /* VGrp0E: group0 virq signaling enabled */
    if ((s->h_hcr[cpu] & R_GICH_HCR_VGRP0EIE_MASK) &&
        (s->cpu_ctlr[vcpu] & GICC_CTLR_EN_GRP0)) {
        value |= R_GICH_MISR_VGrp0E_MASK;
    }

    /* VGrp0D: group0 virq signaling disabled */
    if ((s->h_hcr[cpu] & R_GICH_HCR_VGRP0DIE_MASK) &&
        !(s->cpu_ctlr[vcpu] & GICC_CTLR_EN_GRP0)) {
        value |= R_GICH_MISR_VGrp0D_MASK;
    }

    /* VGrp1E: group1 virq signaling enabled */
    if ((s->h_hcr[cpu] & R_GICH_HCR_VGRP1EIE_MASK) &&
        (s->cpu_ctlr[vcpu] & GICC_CTLR_EN_GRP1)) {
        value |= R_GICH_MISR_VGrp1E_MASK;
    }

    /* VGrp1D: group1 virq signaling disabled */
    if ((s->h_hcr[cpu] & R_GICH_HCR_VGRP1DIE_MASK) &&
        !(s->cpu_ctlr[vcpu] & GICC_CTLR_EN_GRP1)) {
        value |= R_GICH_MISR_VGrp1D_MASK;
    }

    s->h_misr[cpu] = value;
}