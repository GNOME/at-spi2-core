Deprecations in ATK that need work elsewhere
============================================

``atk_document_get_locale()`` and the corresponding field in the vtable
``AtkDocumentIface.get_document_locale`` are documented as deprecated
since 2013 (commit 97880727). However, WebKitGtk still seems to
implement it; it should use ``AtkObjectClass.get_object_locale``
instead.

Note that Orca does not seem to use the ``Document.GetLocale`` method
nor the ``Accessible.Locale`` property, so probably the code above from
WebKitGtk is not exercised.

AtkValue
--------

The following methods are deprecated in ``AtkValueIface`` since 2014
(commit 98838b2a):

-  ``get_current_value`` - replaced with ``get_value_and_text``
-  ``set_current_value`` - replaced with ``set_value``

And the following are replaced with ``get_range``, which returns an
``AtkRange`` boxed type:

-  ``get_maximum_value``
-  ``get_minimum_value``
-  ``get_minimum_increment``

Firefox still does not implement ``get_range``, ``get_value_and_text``,
``set_value``. See bug #71.
