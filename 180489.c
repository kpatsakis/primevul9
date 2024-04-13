static void mark_line_app(cursor *cr, fixed sx, fixed sy, fixed ex, fixed ey)
{
    int isy = fixed2int(sy) - cr->base;
    int iey = fixed2int(ey) - cr->base;

    if (sx == ex && sy == ey)
        return;
#ifdef DEBUG_SCAN_CONVERTER
    dlprintf6("Marking line from %x,%x to %x,%x (%x,%x)\n", sx, sy, ex, ey, isy, iey);
#endif

    assert(cr->y == sy && cr->left <= sx && cr->right >= sx && cr->d >= DIRN_UNSET && cr->d <= DIRN_DOWN);

    /* First, deal with lines that don't change scanline.
     * This accommodates horizontal lines. */
    if (isy == iey) {
        if (sy == ey) {
            /* Horzizontal line. Don't change cr->d, don't flush. */
        } else if (sy > ey) {
            /* Falling line, flush if previous was rising */
            if (cr->d == DIRN_UP)
                output_cursor(cr, sx);
            cr->d = DIRN_DOWN;
        } else {
            /* Rising line, flush if previous was falling */
            if (cr->d == DIRN_DOWN)
                output_cursor(cr, sx);
            cr->d = DIRN_UP;
        }
        if (sx <= ex) {
            if (sx < cr->left)
                cr->left = sx;
            if (ex > cr->right)
                cr->right = ex;
        } else {
            if (ex < cr->left)
                cr->left = ex;
            if (sx > cr->right)
                cr->right = sx;
        }
        cr->y = ey;
    } else if (sy < ey) {
        /* So lines increasing in y. */
        fixed y_steps = ey - sy;
        /* We want to change from sy to ey, which are guaranteed to be on
         * different scanlines. We do this in 3 phases.
         * Phase 1 gets us from sy to the next scanline boundary.
         * Phase 2 gets us all the way to the last scanline boundary.
         * Phase 3 gets us from the last scanline boundary to ey.
         */
        int phase1_y_steps = (fixed_1 - sy) & (fixed_1 - 1);
        int phase3_y_steps = ey & (fixed_1 - 1);

        if (cr->d == DIRN_DOWN)
            output_cursor(cr, sx);
        cr->d = DIRN_UP;

        if (sx <= ex) {
            /* Lines increasing in x. */
            int phase1_x_steps, phase3_x_steps;
            fixed x_steps = ex - sx;

            assert(cr->left <= sx);
            /* Phase 1: */
            if (phase1_y_steps) {
                phase1_x_steps = (int)(((int64_t)x_steps * phase1_y_steps + y_steps/2) / y_steps);
                sx += phase1_x_steps;
                if (cr->right < sx)
                    cr->right = sx;
                x_steps -= phase1_x_steps;
                /* If phase 1 will move us into a new scanline, then we must
                 * flush it before we move. */
                if (fixed2int(cr->y) != fixed2int(cr->y + phase1_y_steps))
                    output_cursor(cr, sx);
                cr->y += phase1_y_steps;
                sy += phase1_y_steps;
                y_steps -= phase1_y_steps;
                if (y_steps == 0)
                    return;
            }

            /* Phase 3: precalculation */
            phase3_x_steps = (int)(((int64_t)x_steps * phase3_y_steps + y_steps/2) / y_steps);
            x_steps -= phase3_x_steps;
            y_steps -= phase3_y_steps;

            /* Phase 2: */
            y_steps = fixed2int(y_steps);
            assert(y_steps >= 0);
            if (y_steps) {
                /* We want to change sx by x_steps in y_steps steps.
                 * So each step, we add x_steps/y_steps to sx. That's x_inc + n_inc/y_steps. */
                int x_inc = x_steps/y_steps;
                int n_inc = x_steps - (x_inc * y_steps);
                int f = y_steps/2;
                int d = y_steps;
                while (y_steps) {
                    cr->left = sx;
                    sx += x_inc;
                    f -= n_inc;
                    if (f < 0)
                        f += d, sx++;
                    cr->right = sx;
                    y_steps--;
                    output_cursor(cr, sx);
                    cr->y += fixed_1;
                }
            }

            /* Phase 3 */
            cr->left  = sx;
            cr->right = ex;
            cr->y += phase3_y_steps;
        } else {
            /* Lines decreasing in x. */
            int phase1_x_steps, phase3_x_steps;
            fixed x_steps = sx - ex;

            assert(cr->right >= sx);
            /* Phase 1: */
            if (phase1_y_steps) {
                phase1_x_steps = (int)(((int64_t)x_steps * phase1_y_steps + y_steps/2) / y_steps);
                sx -= phase1_x_steps;
                if (cr->left > sx)
                    cr->left = sx;
                x_steps -= phase1_x_steps;
                /* If phase 1 will move us into a new scanline, then we must
                 * flush it before we move. */
                if (fixed2int(cr->y) != fixed2int(cr->y + phase1_y_steps))
                    output_cursor(cr, sx);
                cr->y += phase1_y_steps;
                sy += phase1_y_steps;
                y_steps -= phase1_y_steps;
                if (y_steps == 0)
                    return;
            }

            /* Phase 3: precalculation */
            phase3_x_steps = (int)(((int64_t)x_steps * phase3_y_steps + y_steps/2) / y_steps);
            x_steps -= phase3_x_steps;
            y_steps -= phase3_y_steps;

            /* Phase 2: */
            assert((y_steps & (fixed_1 - 1)) == 0);
            y_steps = fixed2int(y_steps);
            assert(y_steps >= 0);
            if (y_steps) {
                /* We want to change sx by x_steps in y_steps steps.
                 * So each step, we sub x_steps/y_steps from sx. That's x_inc + n_inc/ey. */
                int x_inc = x_steps/y_steps;
                int n_inc = x_steps - (x_inc * y_steps);
                int f = y_steps/2;
                int d = y_steps;
                while (y_steps) {
                    cr->right = sx;
                    sx -= x_inc;
                    f -= n_inc;
                    if (f < 0)
                        f += d, sx--;
                    cr->left = sx;
                    y_steps--;
                    output_cursor(cr, sx);
                    cr->y += fixed_1;
                }
            }

            /* Phase 3 */
            cr->right = sx;
            cr->left  = ex;
            cr->y += phase3_y_steps;
        }
    } else {
        /* So lines decreasing in y. */
        fixed y_steps = sy - ey;
        /* We want to change from sy to ey, which are guaranteed to be on
         * different scanlines. We do this in 3 phases.
         * Phase 1 gets us from sy to the next scanline boundary.
         * Phase 2 gets us all the way to the last scanline boundary.
         * Phase 3 gets us from the last scanline boundary to ey.
         */
        int phase1_y_steps = sy & (fixed_1 - 1);
        int phase3_y_steps = (fixed_1 - ey) & (fixed_1 - 1);

        if (cr->d == DIRN_UP)
            output_cursor(cr, sx);
        cr->d = DIRN_DOWN;

        if (sx <= ex) {
            /* Lines increasing in x. */
            int phase1_x_steps, phase3_x_steps;
            fixed x_steps = ex - sx;

            /* Phase 1: */
            assert(cr->left <= sx);
            if (phase1_y_steps) {
                phase1_x_steps = (int)(((int64_t)x_steps * phase1_y_steps + y_steps/2) / y_steps);
                sx += phase1_x_steps;
                if (cr->right < sx)
                    cr->right = sx;
                x_steps -= phase1_x_steps;
                /* Phase 1 in a falling line never moves us into a new scanline. */
                sy -= phase1_y_steps;
                cr->y -= phase1_y_steps;
                y_steps -= phase1_y_steps;
                if (y_steps == 0)
                    return;
            }

            /* Phase 3: precalculation */
            phase3_x_steps = (int)(((int64_t)x_steps * phase3_y_steps + y_steps/2) / y_steps);
            x_steps -= phase3_x_steps;
            y_steps -= phase3_y_steps;

            /* Phase 2: */
            assert((y_steps & (fixed_1 - 1)) == 0);
            y_steps = fixed2int(y_steps);
            assert(y_steps >= 0);
            if (y_steps) {
                /* We want to change sx by x_steps in y_steps steps.
                 * So each step, we add x_steps/y_steps to sx. That's x_inc + n_inc/ey. */
                int x_inc = x_steps/y_steps;
                int n_inc = x_steps - (x_inc * y_steps);
                int f = y_steps/2;
                int d = y_steps;
                while (y_steps) {
                    output_cursor(cr, sx);
                    sx += x_inc;
                    f -= n_inc;
                    if (f < 0)
                        f += d, sx++;
                    cr->right = sx;
                    y_steps--;
                    cr->y -= fixed_1;
                }
            }

            /* Phase 3 */
            if (phase3_y_steps > 0)
                output_cursor(cr, sx);
            cr->left  = sx;
            cr->right = ex;
            cr->y -= phase3_y_steps;
        } else {
            /* Lines decreasing in x. */
            int phase1_x_steps, phase3_x_steps;
            fixed x_steps = sx - ex;

            /* Phase 1: */
            assert(cr->right >= sx);
            if (phase1_y_steps) {
                phase1_x_steps = (int)(((int64_t)x_steps * phase1_y_steps + y_steps/2) / y_steps);
                sx -= phase1_x_steps;
                if (cr->left > sx)
                    cr->left = sx;
                x_steps -= phase1_x_steps;
                /* Phase 1 in a falling line never moves us into a new scanline. */
                sy -= phase1_y_steps;
                cr->y -= phase1_y_steps;
                y_steps -= phase1_y_steps;
                if (y_steps == 0)
                    return;
            }

            /* Phase 3: precalculation */
            phase3_x_steps = (int)(((int64_t)x_steps * phase3_y_steps + y_steps/2) / y_steps);
            x_steps -= phase3_x_steps;
            y_steps -= phase3_y_steps;

            /* Phase 2: */
            y_steps = fixed2int(y_steps);
            assert(y_steps >= 0);
            if (y_steps) {
                /* We want to change sx by x_steps in y_steps steps.
                 * So each step, we sub x_steps/y_steps from sx. That's x_inc + n_inc/ey. */
                int x_inc = x_steps/y_steps;
                int n_inc = x_steps - (x_inc * y_steps);
                int f = y_steps/2;
                int d = y_steps;
                while (y_steps) {
                    output_cursor(cr, sx);
                    sx -= x_inc;
                    f -= n_inc;
                    if (f < 0)
                        f += d, sx--;
                    cr->left = sx;
                    y_steps--;
                    cr->y -= fixed_1;
                }
            }

            /* Phase 3 */
            if (phase3_y_steps > 0)
                output_cursor(cr, sx);
            cr->right = sx;
            cr->left  = ex;
            cr->y -= phase3_y_steps;
        }
    }
}