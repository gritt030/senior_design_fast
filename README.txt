Build using CMAKE:
  cmake CMakeLists.txt
  make
  
  Or open with KDevelop and build it that way


To run:
  ./quadcopter /path/to/dataset/dataset.txt
  
  datasets are in ./datasets directory for each building we took scans in. Note that for walter library datasets,
  west and east sonars are swapped, and so must be manually swapped in the main loop of main.cpp. I've marked this there.
  

Directories:
  datasets: contains data-files with position/sonar data. Format of each line is (all in millimeters):
            Timestamp  x_position_mm  y_position_mm  heading_rad  west_sonar_mm  nw_sonar_mm  ne_sonar_mm  east_sonar_mm
  
  linefitter: contains files for performing hough transform on map and running lsd algorithm on map
  
  localization: contains files for reading in the datasets and turning them into coordinates
  
  occupancygrid: contains files for storing/adding to actual occupancy grid maps
  
  ppmwriter: contains files for writing maps to ppm images
  
  sonar: contains files for storing sonar data and creating maps from it
  
  
Basic process of main.cpp:
  1) Create CoordinateReader to read in data from files in dataset directory and SonarArchive object to store sonar/trajectory info
  2) In loop, read in dataset and store in SonarArchive object as linked list using addSonarScan() function
  3) After loop, generate map with SonarArchive's generateMap() function. Arg is sonar fan angle (0.27 radians is good value).
  4) Perform hough transform on map to see how much it needs to be rotated by (Smith 2 dataset shows this really well).
  5) Rotate the map with SonarArchive's rotate() function.
  6) Re-generate map from rotated sonar data to get straightened map
  7) Run the LSD algorithm to find walls in the image and save them to an occupancy grid
  

Most functions have a "sendToImage" function that can be useful for debugging/seeing what is going on.
Make sure to delete unused occupancy grids manually, any time you generate a new one it gets created with "new" and needs to be released later.
  