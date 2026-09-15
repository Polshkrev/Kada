# Changelog
## v0.4.0 - 2026-09-14
`Added`
- Updated `lib` dependency.
- `process`
    - Added the `process_identifier_t` type.
    - Added the `process_identifier` function.
    - Added the `process_close` function.
    - Added the `process_print` function.
- `command`
    - Added the `command_reserve` function.

`Changed`
- `command`
    - All functions have been changed to prioritize memory safety using crashing states.
    - All includes have been updated.
    - All documentation has been updated.
- `programme`
    - All includes have been updated.
    - All documentation has been updated.
- `process`
    - All includes have been updated.
    - All documentation has been updated.
- `prcesses`
    - All functions have been changed to prioritize memory safety using crashing states.
    - All includes have been updated.
    - All documentation has been updated.

`Fixed`
- `processes`
    - When the `process_array_resize_by` function is called, the array is reallocated now with the correct size.
## v0.3.0 - 2026-09-03
- Updated `lib` dependency.

`Added`
- `process`
    - Added the `current_process` function.
## v0.2.0 - 2026-06-12
`Changed`
- C
    - All structures have been stack allocated where applicable.
## v0.1.0 - 2026-04-04: The Initial Update
`Added`
- Added `command` module.
- Added `programme` module.
- Added `processes` module.
- Added `process` module.
- `Docs`
    - Added all english documentation.