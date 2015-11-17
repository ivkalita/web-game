#ifndef GAME_ENGINE_HPP_INCLUDED
#define GAME_ENGINE_HPP_INCLUDED

#include <iostream>
#include <utility>
#include <cmath>
#include <vector>

namespace Engine {

    typedef double tfloat;
    
    static const tfloat GLOBAL_EPS = 1e-6;


    class Planet {
    private:
        tfloat x, y;
        int ships_num;
        tfloat radius;
        int owner;
        static const tfloat CLOSE_RANGE;
    public:
        Planet(int _ships_num, int _radius) : ships_num(_ships_num), radius(_radius) {}
        
       /* void Receive(Fleet& fleet) {
            if (fleet.GetOwner() != owner) {
                ships_num -= fleet.GetShipsNum();
                if (ships_num < 0) {
                    owner = fleet.GetOwner();
                    ships_num = -ships_num;
                }
            }
            else {
                ships_num += fleet.GetShipsNum();
            }
        }*/

        bool isNear(tfloat _x, tfloat _y) {
            return abs(x - _x) < CLOSE_RANGE && abs(y - _y) < CLOSE_RANGE;
        }

        bool OnTheLine(tfloat x1, tfloat y1, tfloat x2, tfloat y2) {
            tfloat a = x2 - x1, b = y2 - y1;
            tfloat A = a, B = -b, C = b * x1 - a * y1;

            C = C - y + x * b / a;

            return C * C <= radius * radius * (A * A + B * B);
            //c*c > r*r*(a*a + b*b)
        }

        tfloat GetX() {
            return x;
        }

        tfloat GetY() {
            return y;
        }

        int GetOwner() {
            return owner;
        }

        
    };


    class Ship {
    private:
        tfloat x, y, vx, vy, dest_x, dest_y;
        Planet& sender_planet, dest_planet;
        int owner;

        void aim() {
            tfloat dx = dest_x - x, dy = dest_y - y;
            tfloat path_length = sqrt(pow(dx, 2) + pow(dy, 2));
            vx = speed * dx / path_length;
            vy = speed * dy / path_length;
        }

    public:
        static const int speed;

        Ship(Planet& _sender_planet, Planet& _dest_planet, int _owner): 
                dest_planet(_dest_planet), sender_planet(_sender_planet), owner(_owner) {
            
            x = sender_planet.GetX();
            y = sender_planet.GetY();

            aim();
        }

        void Step(std::vector<Planet>& planets) {
            
            bool was_near = false;
            for (auto p : planets) {
                if (p.isNear(x, y) && p.OnTheLine(x, y, dest_x, dest_y)) {
                    tfloat cx = p.GetX() - x;
                    tfloat cy = p.GetY() - y;
                    /* косое произведение двух векторов положительно, 
                    если поворот от первого вектора ко второму идет против часовой стрелки*/
                    if (cx * vy - vx * cy >= 0) {
                        //"слева" от планеты
                        tfloat tmp_x = cx;
                        cx = -cy;
                        cy = tmp_x;
                    }
                    else {
                        tfloat tmp_x = cx;
                        cx = cy;
                        cy = -tmp_x;
                    }

                    tfloat length = sqrt(pow(cx, 2) + pow(cy, 2));
                    vx = cx / length * speed;
                    vy = cy / length * speed;

                    was_near = true;
                    break;
                }   
            }

            if (!was_near)
                aim();
            
            x += vx;
            y += vy;

        }

        int GetOwner() {
            return owner;
        }

    };


    class Engine {
    private:
        std::vector<Ship> ships;
        std::vector<Planet> planets;

    public:
        Engine() {}

        void Step() {
            for (auto& i : ships) {
                i.Step(planets);
            }
        }
    };

};

#endif
