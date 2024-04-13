AP_DECLARE(process_score *) ap_get_scoreboard_process(int x)
{
    if ((x < 0) || (x >= server_limit)) {
        return(NULL); /* Out of range */
    }
    return &ap_scoreboard_image->parent[x];
}