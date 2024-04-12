source = [
  introspection_generated,
  'accessible-adaptor.c',
  'action-adaptor.c',
  'application-adaptor.c',
  'cache-adaptor.c',
  'collection-adaptor.c',
  'component-adaptor.c',
  'document-adaptor.c',
  'editabletext-adaptor.c',
  'hyperlink-adaptor.c',
  'hypertext-adaptor.c',
  'image-adaptor.c',
  'selection-adaptor.c',
  'socket-adaptor.c',
  'table-adaptor.c',
  'table-cell-adaptor.c',
  'text-adaptor.c',
  'value-adaptor.c',
]

libatk_bridge_adaptors = static_library('atk-bridge-adaptors', source,
                                        include_directories: [
                                          root_inc,
                                          include_directories('..'),
                                        ],
                                        dependencies: [ libdbus_dep, atspi_dep, libatk_dep ])

libatk_bridge_adaptors_dep = declare_dependency(link_with: libatk_bridge_adaptors,
                                                dependencies: [ libdbus_dep, atspi_dep, libatk_dep ],
                                                include_directories: include_directories('.'))
