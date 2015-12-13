#ifndef GAME_ENGINE_HPP_INCLUDED
#define GAME_ENGINE_HPP_INCLUDED

#include <iostream>
#include <sstream>
#include <utility>
#include <cmath>
#include <vector>
#include <list>
#include <exception>

namespace GameEngine {

    typedef double tfloat;
    
    static const tfloat GLOBAL_EPS = 1e-6;

    class EngineException : public std::exception {
    private:
        std::string message;
    public:
        EngineException(std::string _message) : message(_message) {};

        virtual const char* what() const throw() {
            return message.c_str();
        }
    };

    class Planet {
    private:
        tfloat x, y;
        tfloat radius;
        int ships_num, owner, id;

        static int id_generator;
        static int gen_id() {
            return Planet::id_generator++;
        }
    public:
        static const tfloat CLOSE_RANGE;

        Planet(tfloat _x, tfloat _y, tfloat _radius, int _ships_num, int _owner) :
            x(_x), y(_y), ships_num(_ships_num), radius(_radius), owner(_owner) {
            id = gen_id();
        }

        // явные конструкторы для отслеживания копирования при работе со сслыками
        Planet(const Planet& a) {
            x = a.x; y = a.y; radius = a.radius;
            ships_num = a.ships_num; owner = a.owner; id = a.id;
        }

        Planet& operator = (const Planet& a) {
            x = a.x; y = a.y; radius = a.radius;
            ships_num = a.ships_num; owner = a.owner; id = a.id;
        }

        bool operator == (const Planet& a) {
            return id == a.id;
        }
        
        bool IsNear(tfloat _x, tfloat _y) {
            return sqrt(pow(x - _x, 2) + pow(y - _y, 2)) < radius + CLOSE_RANGE;
        }

        bool IsInside(tfloat _x, tfloat _y) {
            return sqrt(pow(x - _x, 2) + pow(y - _y, 2)) < radius;
        }

        int ReceiveShips(int count, int ships_owner) {
            if (owner == ships_owner) {
                ships_num += count;
            }
            else {
                ships_num -= count;
                if (ships_num < 0) {
                    owner = ships_owner;
                    ships_num = -ships_num;
                }
            }
            return ships_num;
        }

        int RemoveShips(int count) {
            return ships_num -= count;
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

        int ShipCount() {
            return ships_num;
        }

        tfloat GetRadius() {
            return radius;
        }

        int GetID() {
            return id;
        }

        std::string GetInfo() {
            std::stringstream s;
            s << "X: " << x << " Y: " << y << " radius: " << radius
                << " ships_num: " << ships_num << " owner: " << owner << " id: " << id;
            return s.str();
        }
        
    };


    class Ship {
    private:
        tfloat x, y, vx, vy;
        Planet& sender_planet, &dest_planet;
        int owner;
        bool finished;

        void aim() {
            tfloat dx = dest_planet.GetX() - x, dy = dest_planet.GetY() - y;
            tfloat path_length = sqrt(pow(dx, 2) + pow(dy, 2));
            vx = speed * dx / path_length;
            vy = speed * dy / path_length;
        }

    public:
        static const tfloat speed;

        Ship(Planet& _sender_planet, Planet& _dest_planet): 
                dest_planet(_dest_planet), sender_planet(_sender_planet), finished(false) {
            
            x = sender_planet.GetX() + sender_planet.GetRadius();
            y = sender_planet.GetY();

            owner = sender_planet.GetOwner();

            aim();
        }

        void Step(std::list<Planet>& planets) {

            aim();
            for (auto& p : planets) {
                if (p.IsNear(x, y)) {
                     
                    if (p.IsInside(x + vx, y + vy)) {
                        if (p == dest_planet) {
                            p.ReceiveShips(1, owner);
                            finished = true;
                        }
                        tfloat cx = p.GetX() - x;
                        tfloat cy = p.GetY() - y;

                        if (cx * vy - vx * cy >= 0) {
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

                        break;
                    }
                }   
            }
         
            x += vx;
            y += vy;

        }

        int GetOwner() {
            return owner;
        }

        tfloat GetX() {
            return x;
        }

        tfloat GetY() {
            return y;
        }

        bool Finished() const {
            return finished;
        }

        std::string GetInfo() {
            std::stringstream s;
            s << " X: " << x << " Y: " << y << " VX: " << vx << " VY: " << vy
                << " Sender: " << sender_planet.GetInfo() << " Dest: " << dest_planet.GetInfo()
                << " owner: " << owner << " finished: " << finished;
            return s.str();
        }

    };


    class Engine {
    private:
        std::list<Ship> ships;
        std::list<Planet> planets;

    private:
        void RemoveFinishedFromFront() {
            auto i = ships.cbegin();
            while (i->Finished() && i != ships.end())
                i++;
            ships.erase(ships.cbegin(), i);
        }
    public:
        Engine() {}

        void Step() {
            for (auto& i : ships) {
                if (i.Finished())
                    continue;
                i.Step(planets);

#if (_DEBUG)
                const int MAX_X = 10000;
                const int MAX_Y = 10000;
                for (auto& i : ships) {
                    for (auto& p : planets) {
                        if (p.IsInside(i.GetX(), i.GetY()))
                            throw EngineException("Ship is inside a planet\nShip info: "
                                + i.GetInfo() + "\nPlanet info: " + p.GetInfo() + "\n");
                    }
                    if (abs(i.GetX()) > MAX_X || abs(i.GetY()) > MAX_Y)
                        throw EngineException("Ship flew away\nShip info: " + i.GetInfo());
                }
#endif
            }
            RemoveFinishedFromFront();
        }

        std::list<Ship>& GetShips() {
            return ships;
        }

        std::list<Planet>& GetPlanets() {
            return planets;
        }

        Planet& AddPlanet(tfloat x, tfloat y, tfloat radius, int ships_num, int owner) {
            planets.emplace_back(x, y, radius, ships_num, owner);
            return planets.back();
        }
        
        void Launch(int count, Planet& sender_planet, Planet& dest_planet) {
            if (sender_planet.ShipCount() < count)
                throw EngineException("There are no so many ships (" + std::to_string(count) 
                    + " on a planet:\n" + sender_planet.GetInfo());

            sender_planet.RemoveShips(count);
            for (int i = 0; i < count; i++) {
                ships.emplace_back(sender_planet, dest_planet);
            }
        }

        int ActiveShipsCount() {
            return ships.size();
        }
    };

};

#endif
