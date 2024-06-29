set(
  SOURCE
  create_statement.cpp
  delete_statement.cpp
  explain_statement.cpp
  index_statement.cpp
  insert_statement.cpp
  select_statement.cpp
  update_statement.cpp)

add_library(statement STATIC ${SOURCE})
