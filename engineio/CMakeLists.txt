ADD_SUBDIRECTORY(woody)

SET(engineio_SRCS
  packet.cc
  parser.cc
  server.cc
  socket.cc
  transport.cc 
  )

ADD_LIBRARY(engineio ${engineio_SRCS})

TARGET_LINK_LIBRARIES(engineio woody)
