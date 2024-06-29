set(SOURCE
    bigint_type.cpp
    boolean_type.cpp
    decimal_type.cpp
    integer_parent_type.cpp
    integer_type.cpp
    timestamp_type.cpp
    type.cpp
    value.cpp
    varlen_type.cpp)
add_library(type STATIC ${SOURCE})
