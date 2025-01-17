/*
 * simulate.h
 */

#pragma once

void update_rotation(double **old, double **current, double **next);
double *simulate(const int i_max, const int t_max, const int num_cpus,
        double *old_array, double *current_array, double *next_array);
