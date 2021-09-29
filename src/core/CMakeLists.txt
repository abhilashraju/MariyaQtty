add_library(mariaqtty 
http_session.cpp 
listener.cpp 
shared_state.cpp 
websocket_session.cpp)

file(GLOB APP_FILES
  beast.hpp
  http_session.cpp
  http_session.hpp
  listener.cpp
  listener.hpp
  net.hpp
  shared_state.cpp
  shared_state.hpp
  websocket_session.cpp
  websocket_session.hpp
  chat_client.html
  )

source_group("" FILES ${APP_FILES})
