# Process
This module defines functions that handle a single process.

The two functions that are defined at the moment are `process_wait` and `process_invalid`. Both functions take in a `process_t` by value and return booleans. `process_wait` runs a single `process_t` and waits for its execution to finish. `process_invalid` determines if a given `process_t` is invalid.