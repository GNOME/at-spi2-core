# Deprecations in ATK that need work elsewhere

`atk_document_get_locale()` and the corresponding field in the vtable
`AtkDocumentIface.get_document_locale` are documented as deprecated since 2013 (commit
97880727).  However, WebKitGtk still seems to implement it; it should use
`AtkObjectClass.get_object_locale` instead.

Note that Orca does not seem to use the `Document.GetLocale` method nor the
`Accessible.Locale` property, so probably the code above from WebKitGtk is not exercised.
