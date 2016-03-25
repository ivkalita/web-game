Description
=======================
Basic structure
---------------
Map describes by JSON-file. It contains one global object with map information.

Global object MUST have fields:
* "playersCount" : integer - count of players for this map
* "width": integer - width of map
* "height": integer - height of map
* "planets" : array of objects. Each objec describes the planet

Object that desribes the planet MUST have fields:
* "x" : integer - x-coordinate
* "y" : integer - y-coordinate
* "radius" : integer - radius of planet
* "shipsCount" : integer - count of ships at the start of the game
* "owner" : integer - ID of player who owns the planet

Object that desribes the planet MIGHT have fields:
* "limit" : integer - if planet has more ships than limit, it stops to product new ships. If not defined, default value is radius*1.5
* "production" : number (may be float) - every step ships count increase by this value. If not defined, default value is 0


Notes
-------
* The x-asis is directed from left to right. The y-asis is directed from top to bottom. So the top-left point of game screen has coordinates (0,0), and bottom-right point has coordinates (width, height);
* Planets must not get out of game screen borders;
* The distance between any two planets must be not less than 40, otherwise there can be problems with engine. If the first planet has coordinates (x1,y1) and radius r1, and the second planet has coordinates (x2,y2) and radius r2, distance between this planets can be calculated by formula:  
    *d = sqrt( (x1-x2)^2 + (y1-y2)^2 ) - r1 - r2;*
* Player ID is an integer in range 1..n, where n is count of players. Unplayable player has ID = 0. So neutral planet must have "owner" value = 0.
* Neutral planets don't produce ships.

Example
-------
Here is an example of map with 2 players and 5 planets. First planet is neutral, because its "owner" value is 0.

{  
  "playersCount": 2,  
  "width": 250,  
  "height": 250,  
  "planets": [  
    {  
      "x": 115,  
      "y": 125,  
      "radius": 32,  
      "shipsCount": 50,  
      "owner": 0,  
      "production": 0.05  
    },  
    {  
      "x": 125,  
      "y": 30,  
      "radius": 20,  
      "shipsCount": 20,  
      "owner": 1,  
      "limit": 25,  
      "production": 0.04  
    },  
    {  
      "x": 125,  
      "y": 220,  
      "radius": 20,  
      "shipsCount": 20,  
      "owner": 2,  
      "production": 0.04  
    },  
    {  
      "x": 30,  
      "y": 125,  
      "radius": 20,  
      "shipsCount": 10,  
      "owner": 1,  
      "limit": 30  
    },  
    {  
      "x": 220,  
      "y": 125,  
      "radius": 20,  
      "shipsCount": 10,  
      "owner": 2  
    }  
  ]  
}