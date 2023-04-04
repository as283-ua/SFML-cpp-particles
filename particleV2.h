#ifndef PARTICLE2
#define PARTICLE2

#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>

class Particle : public sf::Drawable{
    friend std::ostream & operator<<(std::ostream &os, const Particle &p){
        os << "posx: " << p.point->position.x << "\tposy: " << p.point->position.y << "\tcolor alpha: " << (int)p.point->color.a << "\taccelx: " << p.acceleration.x << "\taccely: " << p.acceleration.y << "\tspeedx: " << p.speed.x << "\tspeedy: " << p.speed.y << "\tlifetime: " << p.lifetime; 

        return os;
    }
public: 
    sf::VertexArray varr;
    sf::Vertex* point;
    sf::Vector2f speed;
    sf::Vector2f acceleration;
    float lifetime;
    float currentLife = 0;
    static constexpr float defaultLifetime = 1;

public:
    Particle(float x, float y, float speedx, float speedy, float accelerationx, float accelerationy, float lifetime) : varr(sf::PrimitiveType::Points, 1), speed(sf::Vector2f(speedx, speedy)), acceleration(sf::Vector2f(accelerationx, accelerationy)), lifetime(lifetime){
        point = &varr[0];
        point->position.x = x;
        point->position.y = y;
        point->color = sf::Color::White;
    }

    Particle(): varr(sf::PrimitiveType::Points, 1), speed(sf::Vector2f(0, 0)), acceleration(sf::Vector2f(0, 0)), lifetime(2){
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
        acceleration = other.acceleration;
        lifetime = other.lifetime;
        currentLife = other.currentLife;

        return *this;
    }

    bool setLifetime(float lt){
        if(lt < 0){
            return false;
        }

        lifetime = lt;
        return true;
    }

    void move(double time){        
        point->position.x += speed.x*time + 1/2 * time * time * acceleration.x;
        point->position.y += speed.y*time + 1/2 * time * time * acceleration.y;

        speed = speed + acceleration*(float)time;

        if((int)point->color.a - (time/lifetime)*255 < 0){
            point->color.a = 0;
        } else {
            point->color.a -= (time/lifetime)*255;
        }

        currentLife += time;
    }

    private:
    void draw (sf::RenderTarget &target, sf::RenderStates states) const{
        target.draw(varr);
        // std::cout << *this << "\n";
    }

};

#endif