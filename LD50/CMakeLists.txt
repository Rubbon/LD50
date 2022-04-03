cmake_minimum_required (VERSION 3.8)

set (CMAKE_CXX_STANDARD 20)


# get libraries and include them
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
#find_package(ENet REQUIRED)
find_package(OpenAL REQUIRED)
find_package(SndFile REQUIRED)

include_directories(
	${SDL2_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
	#${ENET_INCLUDE_DIRS}
	${OPENAL_INCLUDE_DIR}
	${SNDFILE_INCLUDE_DIR}
)


# Add source to this project's executable.
add_executable (Ld50 "Main.cpp"  "Graphics.h" "Graphics.cpp" "Input.h" "Input.cpp" "Sound.h" "Sound.cpp" "Game.h" "Game.cpp" "Level.h" "Level.cpp" "Entity.h" "Entity.cpp" "Tile.h" "Tile.cpp" "TileBaseInfo.h")


#link the libraries
target_link_libraries(
	Ld50 
	${SDL2_LIBRARIES} 
	${SDL2_IMAGE_LIBRARIES}
	#${ENET_LIBRARIES}
	${OPENAL_LIBRARY}
	${SNDFILE_LIBRARY}
)

# copy resources to output
file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/deps/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#for people who use windows virtual resolution scaling nonesense
target_sources(Ld50 PRIVATE ${CMAKE_SOURCE_DIR}/dpi-aware.manifest)