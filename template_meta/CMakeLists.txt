project( BOOST )
cmake_minimum_required( VERSION 2.6 )

find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIRS} )
set( Boost_USE_STATIC_LIBS        OFF )
set( Boost_USE_MULTITHREADED      ON )
set( Boost_USE_STATIC_RUNTIME     OFF )
set( BOOST_ALL_DYN_LINK           ON )

add_executable( std std.cpp )

add_executable( mpl mpl.cpp )
target_link_libraries( mpl ${Boost_LIBRARIES} )

add_executable( hana hana.cpp )
target_link_libraries( hana ${Boost_LIBRARIES} )
