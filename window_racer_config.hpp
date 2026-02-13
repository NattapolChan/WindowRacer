#pragma once

// #define DETAILED_STATS

// algorithm tuneables
extern double initial_tau; // 0.0001
extern double tau_growth_factor;
const bool mtb_mode = false;
const bool s3a_mode = false;
const int max_num_prev_states = 10;

// debugging
const bool print_debug = false;
const int random_seed = 1;

// hardware parameters
const bool set_affinity = true;
const int cpu_stride = 1;

extern uint64_t num_threads;
extern uint64_t num_entities;

const int phold_events_per_entity = 1;
extern double phold_lambda;
extern double phold_lookahead;
extern double phold_remote_ratio;
extern double phold_zero_delay_ratio;
