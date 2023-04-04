#ifndef CIRCLEPARTICLE
#define CIRCLEPARTICLE

#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>

class CircleParticle : public sf::Drawable{
    // friend std::ostream & operator<<(std::ostream &os, const CircleParticle &p){
    //     os << p.point->position.x << "\t" << p.point->position.y << "\t" << (int)p.point->color.a << "\t" << p.acceleration << "\t" << p.dirVector.x << "\t" << p.dirVector.y; 

    //     return os;
    // }
public: 
    sf::CircleShape point;
    sf::Vector2f dirVector;
    float speed;
    float acceleration;
    float lifetime;
    static constexpr float defaultLifetime = 1;

public:
    CircleParticle(float x, float y, float dirX, float dirY, float speed, float acceleration, float lifetime, float rad) : dirVector(sf::Vector2f(dirX, dirY)), acceleration(acceleration), lifetime(lifetime), point(rad){
        point.setOrigin(point.getPosition().x + 25, point.getPosition().y + 25);
        point.setPosition(x, y);
        point.setFillColor(sf::Color::White);

        this->speed = speed;
    }

    CircleParticle(): dirVector(sf::Vector2f(0, 0)), speed(0), acceleration(0), point(50){
        point = &varr[0];
        point->position.x = 0;
        point->position.y = 0;
        point->color = sf::Color::White;
    }

    ~CircleParticle(){}

    CircleParticle& operator=(const CircleParticle &other){
        varr = other.varr;
        point = &varr[0];
        speed = other.speed;
        dirVector.x = other.dirVector.x;
        dirVector.y = other.dirVector.y;
        acceleration = other.acceleration;

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
    }

    private:
    void draw (sf::RenderTarget &target, sf::RenderStates states) const{
        target.draw(varr);
        // std::cout << *this << "\n";
    }

};

#endif