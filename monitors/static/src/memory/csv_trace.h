#ifndef R2U2_MEMORY_CSV_TRACE_H
#define R2U2_MEMORY_CSV_TRACE_H

#include <stdio.h>
#include <string.h>

#include "internals/errors.h"
#include "internals/types.h"
#include "memory/monitor.h"

// This could arguably have the functions split out as an engine, however they
// are never called by instruction dispatch and may allocate memory so we'll
// contain them here

typedef struct {
  FILE *input_file;

  #if R2U2_PRED_PROB
    char in_buf[R2U2_MAX_SIGNALS*R2U2_MAX_K_MODES*R2U2_MAX_N_PREDICTION_HORIZON*8]; // TODO(bckempa): LINE_MAX instead? PATH_MAX?
  #else
    char in_buf[R2U2_MAX_SIGNALS*8]; // TODO(bckempa): LINE_MAX instead? PATH_MAX?
  #endif

  r2u2_bool as_atomics; // if true, load csv data directly into atomics vector

} r2u2_csv_reader_t;

r2u2_status_t r2u2_csv_load_next_signals(r2u2_csv_reader_t *trace_reader, r2u2_csv_reader_t *prob_reader, r2u2_monitor_t *monitor);

#endif /* R2U2_MEMORY_CSV_TRACE_H */
