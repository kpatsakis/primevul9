gx_edgebuffer_fin(gx_device     * pdev,
                  gx_edgebuffer * edgebuffer)
{
    gs_free_object(pdev->memory, edgebuffer->table, "scanc intersects buffer");
    gs_free_object(pdev->memory, edgebuffer->index, "scanc index buffer");
    edgebuffer->index = NULL;
    edgebuffer->table = NULL;
}