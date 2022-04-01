project( template_meta )
cmake_minimum_required( VERSION 2.6 )

set(BOOST_ROOT /mnt/c/Users/Gergely/Documents/Private/Git/Generative_Programming/boost_1_77_0/)
find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIRS} )
set( Boost_USE_STATIC_LIBS        OFF )
set( Boost_USE_MULTITHREADED      ON )
set( Boost_USE_STATIC_RUNTIME     OFF )
set( BOOST_ALL_DYN_LINK           ON )

add_executable( std.out std.cpp )

add_executable( mpl.out mpl.cpp )
target_link_libraries( mpl.out ${Boost_LIBRARIES} )

add_executable( hana.out hana.cpp )
target_link_libraries( hana.out ${Boost_LIBRARIES} )
