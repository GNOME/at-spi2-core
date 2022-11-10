Notes on the device event controller machinery
----------------------------------------------

In pyatspi2, `Registry.setReferenceWIndow()` is misspelled.  However,
that function is never called in Orca or elsewhere, as far as I can
tell.

This ties down to `atspi_registry_set_reference_window()`, which just
calls `_atspi_mutter_set_reference_window()`.  The side effect of that
is to set the `ATSPI_MUTTER_DATA.window_id` and
`window_id_is_explicit`, but again, those fields never get used in the
`atspi-mutter.c` code.  I think they were meant to be used in
`_atspi_mutter_generate_keyboard_event` or
`_atspi_mutter_generate_mouse_event`.

