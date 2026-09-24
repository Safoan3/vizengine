

cmake_minimum_required(VERSION 3.23)

project(vizengine)


#add_compile_options(-g -O0 -march=native)   # temporarily, for debugging

add_compile_options( -O2 -march=native)   # temporarily, for debugging




#add_executable(engineprogramme)

add_library(engine)



target_sources(engine PRIVATE
	  ./engine/engine.cpp
    ./raycasting/raycast.cpp
    ../../graphicsapi/graphicsapi.cpp
    ./libs/miniaud/wrapper.cpp

    #rend 
    ./rendering/RenderingMethod/dot.cpp
    ./rendering/RenderingMethod/wireframe.cpp
    ./rendering/RenderingMethod/rasterizing.cpp
    ./rendering/RenderingMethod/zbuffdebug.cpp
    ./rendering/RenderingMethod/raytracing.cpp
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
    ./rendering/RenderingMethod
  )




