

cmake_minimum_required(VERSION 3.23)

project(vizengine)


#add_compile_options(-g -O0 -march=native)   # temporarily, for debugging
add_compile_options( -g -O0 -march=native)   # temporarily, for debugging




add_executable(engineprogramme)
add_library(engine)


target_sources(engineprogramme PRIVATE
	  ./engine/engine.cpp
    ./raycasting/raycast.cpp
    ./rendering/render.cpp
    ../../graphicsapi/graphicsapi.cpp
  )


target_include_directories(engineprogramme PRIVATE
    ./engine
    ./raycasting
    ./rendering
    ../../graphicsapi
    ./
    ./objects
    ./math
    ./libs/imgloader
    ./libs/miniaud
  )



  
target_sources(engineprogramme PRIVATE
	  ./testing/fps.cpp
  )





target_sources(engine PRIVATE
	  ./engine/engine.cpp
    ./raycasting/raycast.cpp
    ./rendering/render.cpp
    ../../graphicsapi/graphicsapi.cpp
  )



  target_include_directories(engine PUBLIC
    ./engine
    ./raycasting
    ./rendering
    ../../graphicsapi
    ./
    ./objects
    ./math
    ./libs/imgloader
    ./libs/miniaud
  )

