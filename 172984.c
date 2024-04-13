pdf14_free_smask_color(pdf14_device * pdev)
{
    if (pdev->smaskcolor != NULL) {
        if ( pdev->smaskcolor->profiles != NULL) {
            /* Do not decrement the softmask enties.  They will remain
               in the icc_manager softmask member.  They were not
               incremented when moved here */
            gs_free_object(pdev->memory, pdev->smaskcolor->profiles,
                        "pdf14_free_smask_color");
        }
        gs_free_object(pdev->memory, pdev->smaskcolor, "pdf14_free_smask_color");
        pdev->smaskcolor = NULL;
    }
}