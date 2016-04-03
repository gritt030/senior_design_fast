add_library(navigate  virtualenvironment.cpp
                      navigator.cpp
                      navigationmap/navigationmap.cpp
                      navigationmap/grid/opengrid.cpp
                      navigationmap/grid/frontiergrid.cpp
                      )

add_library(navigation_map  navigationmap/navigationmap.cpp
                            navigationmap/grid/opengrid.cpp
                            navigationmap/grid/frontiergrid.cpp
                            )
