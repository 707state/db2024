set(SOURCE
    binder.cpp
    bind_create.cpp
    bind_insert.cpp
    bind_select.cpp
    bind_drop.cpp
    bind_variable.cpp
    bound_statement.cpp
    fmt_impl.cpp
    keyword_helper.cpp
    node_tag_to_string.cpp
    transformer.cpp)
add_subdirectory(statement)
add_library(binder OBJECT ${SOURCE})
