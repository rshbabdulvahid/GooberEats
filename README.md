# GooberEats
Delivery Planner Project written in C++

Usage: In the same location as the source files, create a text file formatted as follows:

          DELIVERY_COORD_FROM_mapdata.txt: item_name (string) \n
          DELIVERY_COORD_FROM_mapdata.txt: item_name (string) \n
          ...
          
       Then, in file "main.cpp", line 45, where it says "if (!loadDeliveryRequests("YOUR_FILE_HERE", depot, deliveries))", place your file name in the indicated
       location.

This project utilizes a text file consisting of pairs of coordinate values (representing real map coordinate data of the Los Angeles area) and, from that
text file, builds a map structure with internal representation of street segments as well as the angles between streets. The project features multiple files,
including:

  -ExpandableHashMap: An extendable hash map built using templates
  
  -StreetMap: 
      This code takes the information presented in the file, mapdata.txt, and creates a graph structure from it, using the expandable hash map
      built prior.
      
  -PointToPoint: 
      This code uses the graph structure built from StreetMap and using A* pathfinding in order to construct a shortest-distance path, traveling
      through delivery locations (specified in a user-inputted file). It produces as output a list of street segments, which are a series of geographical
      coordinate pairs with street names associated to them.
      
  -DeliveryOptimizer: 
      This code combats the traveling salesman problem exposed by this problem. The optimal order of deliveries could vary based on features of the
      coordinates and their locations relative to one another. This code uses a simulated annealing algorithm which capitalizes on randomness in order to
      calculate a pseudo-optimal delivery order. It uses straight-line distance between coordinate locations as a heuristic for path length.
      
  -DeliveryPlanner:
      This code takes the list of street segments outputted by PointToPoint and converts it into directions that are more easily understandable to a human driver.
      In this format, it is finally ready to be outputted. Sample output is shown in the file test_output.png:
      

![Alt text](https://github.com/rshbabdulvahid/GooberEats/blob/master/test_output.PNG) 
