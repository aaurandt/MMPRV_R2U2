#include "r2u2.h"
// We use a config flag and therefore must include the toplevel header first

#include "csv_trace.h"

r2u2_status_t r2u2_csv_load_next_signals(r2u2_csv_reader_t *trace_reader, r2u2_csv_reader_t *prob_reader, r2u2_monitor_t *monitor) {

  char *signal;
  // Since this can store a pointer, it must be able to store an index
  uintptr_t i;
  uintptr_t k = 0;

  // Read in next line of trace to internal buffer for processing
  if(fgets(trace_reader->in_buf, sizeof(trace_reader->in_buf), trace_reader->input_file) == NULL) return R2U2_END_OF_TRACE;

    // Skip header row, if it exists - note we only check for this on the first line
    if (monitor->time_stamp == 0 && trace_reader->in_buf[0] == '#') {
      if(fgets(trace_reader->in_buf, sizeof(trace_reader->in_buf), trace_reader->input_file) == NULL) return R2U2_END_OF_TRACE;
    }

    #if R2U2_CSV_Header_Mapping

    // TODO(bckempa): Port header mapping code
    for(i = 0, signal = strtok(trace_reader->in_buf, ",\n"); signal; i++, signal = strtok(NULL, ",\n")) {
      // Follow the pointer to the signal vector, then assign the ith element
      // Note this is a pointer into the r2u2_csv_reader_t in_buf which must
      // stay in place while the signal vector is live
      (*(monitor->signal_vector))[i] = signal;
    }

    #else

    if (trace_reader->as_atomics) {
      for(i = 0, signal = strtok(trace_reader->in_buf, ",\n"); signal; i++, signal = strtok(NULL, ",\n")) {
        #if R2U2_PRED_PROB
          // Check if starting next mode of prediction and store in offset buffer
          if(strcmp(signal, "|") == 0){
            if(k == 0){
              monitor->num_atomics = i;
            }
            if(k < R2U2_MAX_K_MODES)
              (*(monitor->k_offset_buffer)[1])[k] = i;
            k++;
            i--;
            continue;
          }
        #endif
        // Follow the pointer to the current atomic buffer, then assign the ith element
        // Note this is a pointer into the r2u2_csv_reader_t in_buf which must
        // stay in place while the atomic buffer is live
        int temp;
        sscanf(signal, "%d", &temp);
        (*(monitor->atomic_buffer)[0])[i] = temp;
      }
    } else {
      for(i = 0, signal = strtok(trace_reader->in_buf, ",\n"); signal; i++, signal = strtok(NULL, ",\n")) {
        #if R2U2_PRED_PROB
          // Check if starting next mode of prediction and store in offset buffer
          if(strcmp(signal, "|") == 0){
            if(k < R2U2_MAX_K_MODES)
              (*(monitor->k_offset_buffer)[0])[k] = i;
            k++;
            i--;
            continue;
          }
        #endif
        // Follow the pointer to the signal vector, then assign the ith element
        // Note this is a pointer into the r2u2_csv_reader_t in_buf which must
        // stay in place while the signal vector is live
        (*(monitor->signal_vector))[i] = signal;
      }
    }

    #endif
  
  #if R2U2_PRED_PROB
  k = 0;
  if (prob_reader->input_file != NULL){
    // Read in next line of trace probabilities to internal buffer for processing
    if(fgets(prob_reader->in_buf, sizeof(prob_reader->in_buf), prob_reader->input_file) == NULL) return R2U2_END_OF_TRACE;

      // Skip header row, if it exists - note we only check for this on the first line
      if (monitor->time_stamp == 0 && prob_reader->in_buf[0] == '#') {
        if(fgets(prob_reader->in_buf, sizeof(prob_reader->in_buf), prob_reader->input_file) == NULL) return R2U2_END_OF_TRACE;
      }

      #if R2U2_CSV_Header_Mapping

      // TODO(bckempa): Port header mapping code
      for(i = 0, signal = strtok(prob_reader->in_buf, ",\n"); signal; i++, signal = strtok(NULL, ",\n")) {
        // Follow the pointer to the signal vector, then assign the ith element
        // Note this is a pointer into the r2u2_csv_reader_t in_buf which must
        // stay in place while the signal vector is live
        if (strcmp(signal, "|") == 0){
          (*(monitor->atomic_prob_buffer))[i] = 1000.0;
        }else{
          sscanf(signal, "%lf", &prob);
          (*(monitor->atomic_prob_buffer))[i] = prob;
        }
      }

      #else

      for(i = 0, signal = strtok(prob_reader->in_buf, ",\n"); signal; i++, signal = strtok(NULL, ",\n")) {
        // Check if starting next mode of prediction and store in offset buffer
        // Note: Technically don't need to restore in k_offset_buffer if trace_reader->as_atomics
        if(strcmp(signal, "|") == 0){
          if(k < R2U2_MAX_K_MODES)
            (*(monitor->k_offset_buffer)[1])[k] = i;
          k++;
          i--;
        }else{
          // Follow the pointer to the atomic probability vector, then assign the ith element
          // Note this is a pointer into the r2u2_csv_reader_t in_buf which must
          // stay in place while the atomic probability vector is live
          r2u2_float prob;
          sscanf(signal, "%lf", &prob);
          (*(monitor->atomic_prob_buffer))[i] = prob;
        }
      }

      #endif
  }
  #endif

  R2U2_DEBUG_PRINT("Returning here....\n");
  return R2U2_OK;
}
