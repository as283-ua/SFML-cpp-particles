#ifndef PARTICLE
#define PARTICLE

#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>

class Particle : public sf::Drawable{
    friend std::ostream & operator<<(std::ostream &os, const Particle &p){
        os << p.point->position.x << "\t" << p.point->position.y << "\t" << (int)p.point->color.a << "\t" << p.acceleration << "\t" << p.dirVector.x << "\t" << p.dirVector.y; 

        return os;
    }
public: 
    sf::VertexArray varr;
    sf::Vertex* point;
    sf::Vector2f dirVector;
    float speed;
    float acceleration;
    float lifetime;
    static constexpr float defaultLifetime = 1;

public:
    Particle(float x, float y, float dirX, float dirY, float speed, float acceleration, float lifetime) : dirVector(sf::Vector2f(dirX, dirY)), acceleration(acceleration), varr(sf::PrimitiveType::Points, 1), lifetime(lifetime){
        point = &varr[0];
        point->position.x = x;
        point->position.y = y;
        point->color = sf::Color::White;

        this->speed = speed;
    }

    Particle(): varr(sf::PrimitiveType::Points, 1), dirVector(sf::Vector2f(0, 0)), speed(0), acceleration(0), lifetime(2){
        point = &varr[0];
        point->position.x = 0;
        point->position.y = 0;
        point->color = sf::Color::White;
    }

    ~Particle(){}

    Particle& operator=(const Particle &other){
        varr = other.varr;
        point = &varr[0];
        speed = other.speed;
        dirVector.x = other.dirVector.x;
        dirVector.y = other.dirVector.y;
        acceleration = other.acceleration;
        lifetime = other.lifetime;

        return *this;
    }

    bool setLifetime(float lt){
        if(lt < 0){
            return false;
        }

        lifetime = lt;
        return true;
    }

    double dirHipotenuse(){
        return sqrt(dirVector.x * dirVector.x + dirVector.y * dirVector.y);
    }

    void move(double time){
        double hipo = dirHipotenuse();
        float xDelta = dirVector.x/hipo;
        float yDelta = dirVector.y/hipo;
        
        point->position.x += xDelta*(speed*time + 1/2 * time * time * acceleration);
        point->position.y += yDelta*(speed*time + 1/2 * time * time * acceleration);

        speed = speed + acceleration*time;

        if((int)point->color.a - (time/lifetime)*255 < 0){
            point->color.a = 0;
        } else {
            point->color.a -= (time/lifetime)*255;
        }

        // dirVector.y += 200;
    }

    private:
    void draw (sf::RenderTarget &target, sf::RenderStates states) const{
        target.draw(varr);
        // std::cout << *this << "\n";
    }

};

#endif