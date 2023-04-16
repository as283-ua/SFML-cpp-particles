//              TODO:
//  fix limitbounds and center when modifying view


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <chrono>
#include <random>
#include <tuple>
#include <iostream>
#include <fstream>
#include <string>
#include "particleV2.h"
#include "color.h"

using namespace std::chrono;

//config
uint WINDOWX = 800;
uint WINDOWY = 600;
float MAXSPD = 350;
float GRAVITYMULTIPLIER = 3;
bool ALWAYSTOCENTER = 0;
bool ALWAYSTOCENTERINVERSE = 0;
bool TOCENTERGRADUAL = 0;
bool LIMITTOBOUNDS = 0;
enum accelerateTo{
    ORIGIN,
    CIRCLES,
    CENTER,
    CENTERREVERSE,
    UPDOWN,
    CENTERINVERSEPOWER
};
int ACCELERATEN = 6;
accelerateTo ACCELTO = ORIGIN;
bool COLORFADE = 0;
bool LONGLIFETIME = 0;
bool SHOWGRAVITYPTS = 0;


const uint SCREENY = sf::VideoMode::getDesktopMode().height;
const uint SCREENX = sf::VideoMode::getDesktopMode().width;

sf::Mouse mouse;
sf::RenderWindow window(sf::VideoMode(WINDOWX, WINDOWY), "particles");
sf::Color mainColor = sf::Color::White;

sf::Sound bop;
std::vector<sf::Sound> bops(5, sf::Sound());
std::vector<sf::SoundBuffer> bopBuffers(5, sf::SoundBuffer());


class GravityPoint{
    public:
        static const float RADIUS;
        sf::CircleShape circleIndicator = sf::CircleShape(RADIUS);
        
        GravityPoint(float x, float y){
            circleIndicator.setOrigin(RADIUS, RADIUS);
            circleIndicator.setPosition(x, y);
        }

        sf::Vector2f getPosition(){
            return circleIndicator.getPosition();
        }
};
const float GravityPoint::RADIUS = 10;


std::vector<GravityPoint> gravPoints;

double getTime(){
    milliseconds timems = duration_cast< milliseconds >(
        system_clock::now().time_since_epoch()
    );

    return ((double)timems.count())/1000;
}

uint checkArgs(int argc, char** argv){
    if(argc == 1){
        return 5;
    }

    if(argc == 2){
        return std::min((uint)atoi(argv[1]), (uint)10000);
    }

    std::cout << "Incorrect params: max 1 param, particleCount\n";
    exit(1);
}

float hypotenuse(float x, float y){
    return sqrt(x*x + y*y);
}

bool isClose(float x1, float y1, float x2, float y2){
    return (abs(x1 - x2) < 20) && (abs(y1 - y2) < 20);
}

void setDefaultParticle(Particle& p, float posX, float posY){
    p.currentLife = 0;
    //INITIAL POS
    if(LIMITTOBOUNDS == 1){
        float limLeft = window.getView().getCenter().x - window.getView().getSize().x/2,
                limRight = window.getView().getCenter().x + window.getView().getSize().x/2,
                limUp = window.getView().getCenter().y - window.getView().getSize().y/2,
                limDown = window.getView().getCenter().y + window.getView().getSize().y/2;

        if (limLeft <= posX && posX <= limRight)
        {
            p.point->position.x = posX;
        } else if (posX < limLeft)
        {
            p.point->position.x = limLeft;
        } else {
            p.point->position.x = limRight - 1;
        }

        if (limUp <= posY && posY <= limDown)
        {
            p.point->position.y = posY;
        } else if (posY < limUp)
        {
            p.point->position.y = limUp;
        } else {
            p.point->position.y = limDown - 1;
        }
    } else {
        p.point->position.x = posX;
        p.point->position.y = posY;
    }

    //SPEED
    float maxY;
    p.speed.x = rand()%((int)MAXSPD*2)-MAXSPD;
    maxY = sqrt(MAXSPD*MAXSPD - p.speed.x*p.speed.x);

    if (maxY == 0)
    {
        p.speed.y = 0;
    } else {
        p.speed.y = rand()%((int)maxY*2)-maxY;
    }


    //lifeTIME
    if (LONGLIFETIME == 1)
    {
        p.lifetime = 1000;
    } else {
        p.lifetime = (float)(rand()%4000)/1000 + 1;
    }
    


    //WHERE TO ACCELERATE
    uint centerx = window.getSize().x/2, centery = window.getSize().y/2;
    float distXtoCenter = centerx - p.point->position.x,
            distYtoCenter = centery - p.point->position.y;
    float centerDist = hypotenuse(distXtoCenter, distYtoCenter);

    switch(ACCELTO){
    case ORIGIN:
        p.acceleration.x = -1*p.speed.x/p.lifetime*GRAVITYMULTIPLIER;
        p.acceleration.y = -1*p.speed.y/p.lifetime*GRAVITYMULTIPLIER;
        break;
    case CIRCLES:
        p.acceleration.x = 98*GRAVITYMULTIPLIER*std::cos(5*getTime());
        p.acceleration.y = 98*GRAVITYMULTIPLIER*std::sin(5*getTime());
        break;
    case CENTER:
        p.acceleration.x = 20000/centerDist*GRAVITYMULTIPLIER*distXtoCenter/centerDist;
        p.acceleration.y = 20000/centerDist*GRAVITYMULTIPLIER*distYtoCenter/centerDist;
        break;
    case CENTERREVERSE:
        p.acceleration.x = -20000/centerDist*GRAVITYMULTIPLIER*distXtoCenter/centerDist;
        p.acceleration.y = -20000/centerDist*GRAVITYMULTIPLIER*distYtoCenter/centerDist;
        break;
    case UPDOWN:
        p.acceleration.x = 0;
        p.acceleration.y = 98*GRAVITYMULTIPLIER*std::sin(5*getTime());
        break;
    case CENTERINVERSEPOWER:
        p.acceleration.x = (centerx - p.point->position.x)*GRAVITYMULTIPLIER;
        p.acceleration.y = (centery - p.point->position.y)*GRAVITYMULTIPLIER;


    default:
        break;
    }


    //COLOR
    p.point->color = mainColor;
    mainColor = nextcolor(mainColor);
}

sf::Vector2f getAccurateMousePos(){
    float ratio = window.getView().getSize().x/(float)window.getSize().x;
    float x = ratio*(mouse.getPosition().x - window.getPosition().x) - (ratio-1)*window.getView().getCenter().x;
    float y = ratio*(mouse.getPosition().y - window.getPosition().y - 37) - (ratio-1)*window.getView().getCenter().y;
    return sf::Vector2f(x, y);
}

void initCluster(Particle* pc, uint size){
    sf::Vector2f mousePos;
    for (size_t i = 0; i < size; i++)
    {
        mousePos = getAccurateMousePos();
        setDefaultParticle(pc[i], mousePos.x, mousePos.y);
    }
}

void updateCluster(Particle* pc, uint size, double time){
    sf::Vector2f mousePos;
    for (size_t i = 0; i < size; i++)
    {
        if (LONGLIFETIME != 1 && pc[i].lifetime > 5)
        {
            pc[i].lifetime = (float)(rand()%4000)/1000 + 1;
        }
        
        if(pc[i].currentLife > pc[i].lifetime){
            mousePos = getAccurateMousePos();
            setDefaultParticle(pc[i], mousePos.x, mousePos.y);

        } else {
            pc[i].move(time);

            if (LIMITTOBOUNDS)
            {
                float limLeft = window.getView().getCenter().x - window.getView().getSize().x/2,
                    limRight = window.getView().getCenter().x + window.getView().getSize().x/2,
                    limUp = window.getView().getCenter().y - window.getView().getSize().y/2,
                    limDown = window.getView().getCenter().y + window.getView().getSize().y/2;

                bool collided = false;

                if(pc[i].point->position.x < limLeft){
                    pc[i].point->position.x = limLeft;

                    //reverse speed and acceleration when hitting wall
                    pc[i].speed.x = -pc[i].speed.x/2;
                    pc[i].speed.y = pc[i].speed.y/2;
                    pc[i].acceleration.x = -pc[i].acceleration.x/2;
                    pc[i].acceleration.y = pc[i].acceleration.y/2;
                    collided = true;
                } else if (pc[i].point->position.x >= limRight)
                {
                    pc[i].point->position.x = limRight - 1;

                    pc[i].speed.x = -pc[i].speed.x/2;
                    pc[i].speed.y = pc[i].speed.y/2;
                    pc[i].acceleration.x = -pc[i].acceleration.x/2;
                    pc[i].acceleration.y = pc[i].acceleration.y/2;
                    collided = true;
                }

                if(pc[i].point->position.y < limUp){
                    pc[i].point->position.y = limUp;

                    pc[i].speed.x = pc[i].speed.x/2;
                    pc[i].speed.y = -pc[i].speed.y/2;
                    pc[i].acceleration.x = pc[i].acceleration.x/2;
                    pc[i].acceleration.y = -pc[i].acceleration.y/2;

                    collided = true;
                } else if (pc[i].point->position.y >= limDown)
                {
                    pc[i].point->position.y = limDown - 1;

                    pc[i].speed.x = pc[i].speed.x/2;
                    pc[i].speed.y = -pc[i].speed.y/2;
                    pc[i].acceleration.x = pc[i].acceleration.x/2;
                    pc[i].acceleration.y = -pc[i].acceleration.y/2;

                    collided = true;
                }

                if (collided){
                    unsigned rnd = rand()%10;
                    if(rnd < bops.size()){
                        bops[rnd].play();
                    }
                } 
            }           

            if (COLORFADE != 1)
            {
                pc[i].point->color.a = 255;
            }
            
            float accelX = 0, accelY = 0;
            for (size_t j = 0; j < gravPoints.size(); j++)
            {
                float distX = gravPoints[j].getPosition().x - pc[i].point->position.x,
                        distY = gravPoints[j].getPosition().y - pc[i].point->position.y;
                float centerDist = hypotenuse(distX, distY);


                accelX += 10*GRAVITYMULTIPLIER*(distX/centerDist);
                accelY += 10*GRAVITYMULTIPLIER*(distY/centerDist);
            }
            pc[i].acceleration.x = accelX;
            pc[i].acceleration.y = accelY;

            float centerx = window.getSize().x/2;
            float centery = window.getSize().y/2;
            if (ALWAYSTOCENTERINVERSE == 1)
            {
                pc[i].acceleration.x = (centerx - pc[i].point->position.x)*GRAVITYMULTIPLIER;
                pc[i].acceleration.y = (centery - pc[i].point->position.y)*GRAVITYMULTIPLIER;
            }
        }
    }
}

void drawCluster(sf::RenderWindow &window, Particle* pc, uint size){
    for (size_t i = 0; i < size; i++)
    {
        window.draw(pc[i]);
    }
}

void config(){
    std::ifstream file("config", std::ios::in);
    if (file.is_open())
    {
        std::string str;
        std::getline(file, str);
        WINDOWX = std::stoi(str);
        std::getline(file, str);
        WINDOWY = std::stoi(str);
        std::getline(file, str);
        MAXSPD = std::stof(str);
        std::getline(file, str);
        GRAVITYMULTIPLIER = std::stof(str);


        std::getline(file, str);
        ALWAYSTOCENTER = std::stoi(str);
        std::getline(file, str);
        TOCENTERGRADUAL = std::stoi(str);
        std::getline(file, str);
        LIMITTOBOUNDS = std::stoi(str);
        std::getline(file, str);
        ACCELTO = (accelerateTo)std::stoi(str);
        std::getline(file, str);
        COLORFADE = (accelerateTo)std::stoi(str);
        std::getline(file, str);
        LONGLIFETIME = (accelerateTo)std::stoi(str);
        std::getline(file, str);
        SHOWGRAVITYPTS = (accelerateTo)std::stoi(str);
        file.close();
    } else {
        WINDOWX = 800;
        WINDOWY = 600;
    }
    window.setSize(sf::Vector2u(WINDOWX, WINDOWY));
}

void printview(sf::View v){
    std::cout << v.getCenter().x << " " << v.getCenter().y << " " << v.getSize().x << " " << v.getSize().y << "\n";
}

void checkEvent(sf::Event e, const int particleCountMax, int &particleCount){
    sf::Vector2f windowViewCenter, windowViewSize;
    float scrollSize;
    sf::Vector2i prevMousePos = sf::Mouse::getPosition();
    sf::Vector2f accmousepos = getAccurateMousePos();
    std::vector<uint> removeGravPtsIndeces;

    switch (e.type)
    {
    case sf::Event::EventType::Closed:
        window.close();
        break;
    
    case sf::Event::EventType::Resized:
        window.setView(sf::View(sf::FloatRect(0, 0, e.size.width, e.size.height)));
        break;
    case sf::Event::EventType::MouseWheelScrolled:
        windowViewCenter = window.getView().getCenter();
        windowViewSize = window.getView().getSize();
        scrollSize = 1.2;
        if(e.mouseWheelScroll.delta > 0 && windowViewSize.x > 100 && windowViewSize.y > 100){
            // printview(window.getView());
            window.setView( sf::View(windowViewCenter,
                            sf::Vector2f(windowViewSize.x / scrollSize, 
                                windowViewSize.y / scrollSize)));
            // printview(window.getView());
        } else {
            window.setView( sf::View(windowViewCenter, 
                            sf::Vector2f(windowViewSize.x * scrollSize, 
                                windowViewSize.y * scrollSize)));
        }
        break;
    case sf::Event::EventType::MouseMoved:
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle)){
            windowViewCenter = window.getView().getCenter();
            windowViewSize = window.getView().getSize();

            float newPosX = windowViewCenter.x + e.mouseMove.x - prevMousePos.x + window.getPosition().x;
            float newPosY = windowViewCenter.y + e.mouseMove.y - prevMousePos.y + window.getPosition().y + 37;

            window.setView(sf::View(sf::Vector2f(newPosX, newPosY), windowViewSize));
        }
        break;
    case sf::Event::EventType::KeyPressed:
        if(e.key.code == sf::Keyboard::V){
            COLORFADE = !COLORFADE;
        } else if (e.key.code == sf::Keyboard::C)
        {
            ALWAYSTOCENTER = !ALWAYSTOCENTER;
            ALWAYSTOCENTERINVERSE = false;
        } else if (e.key.code == sf::Keyboard::B)
        {
            LIMITTOBOUNDS = !LIMITTOBOUNDS;
        } else if (e.key.code == sf::Keyboard::M)
        {
            ACCELTO = (accelerateTo)((ACCELTO + 1)%ACCELERATEN);
        } else if (e.key.code == sf::Keyboard::I)
        {
            ALWAYSTOCENTERINVERSE = !ALWAYSTOCENTERINVERSE;
            ALWAYSTOCENTER = false;
        } else if (e.key.code == sf::Keyboard::Up)
        {
            GRAVITYMULTIPLIER++;
        } else if (e.key.code == sf::Keyboard::Down)
        {
            GRAVITYMULTIPLIER--;
        } else if (e.key.code == sf::Keyboard::Left)
        {
            particleCount -= 100;
            if (particleCount < 10)
            {
                particleCount = 10;
            }
        } else if (e.key.code == sf::Keyboard::Right)
        {
            particleCount += 100;
            if (particleCount > particleCountMax)
            {
                particleCount = particleCountMax;
            }
            
        } else if (e.key.code == sf::Keyboard::S)
        {
            SHOWGRAVITYPTS = !SHOWGRAVITYPTS;
        } else if (e.key.code == sf::Keyboard::L)
        {
            LONGLIFETIME = !LONGLIFETIME;
        }
        break;
    case sf::Event::EventType::MouseButtonPressed:
        if(e.mouseButton.button == sf::Mouse::Button::Left){
            gravPoints.push_back(GravityPoint(accmousepos.x, accmousepos.y));
            // for (size_t i = 0; i < gravPoints.size(); i++)
            // {
            //     std::cout << gravPoints[i].getPosition().x << " " << gravPoints[i].getPosition().y << " - ";
            // }
            // std::cout << std::endl;
            
        } else if(e.mouseButton.button == sf::Mouse::Button::Right) {
            for (size_t i = 0; i < gravPoints.size(); i++)
            {
                if (isClose(gravPoints[i].getPosition().x, gravPoints[i].getPosition().y,
                            accmousepos.x, accmousepos.y))
                {
                    removeGravPtsIndeces.push_back(i);
                }
            }

            for (int i = removeGravPtsIndeces.size() - 1; i >= 0; i--)
            {
                gravPoints.erase(gravPoints.begin() + removeGravPtsIndeces[i]);
            }
            
        }
        break;
    default:
        break;
    }
}

void printControls(){
    std::cout << "M-\tChange mode\n" << 
                "C-\tToggle always to center\n" <<
                "I-\tAlways to center Inverse\n" <<
                "B-\tToggle bound to border\n" <<
                "V-\tToggle color fade\n" <<
                "S-\tShow center\n" <<
                "L-\tLong lifetime toggle\n" <<
                "up/down arrows-\tIncrease/decrease gravity multiplier\n" <<
                "left/right arrows-\tIncrease/decrease emited particles\n";
}

int main(int argc, char** argv){
    config();
    
    window.setPosition(sf::Vector2i(SCREENX/2-window.getSize().x/2, SCREENY/2-window.getSize().y/2));
    // window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    srand(getTime());

    int particleCountMax, particleCount;
    particleCountMax = checkArgs(argc, argv);

    for (size_t i = 0; i < bops.size(); i++)
    {
        sf::SoundBuffer& soundbuffer = bopBuffers[i];

        if (!soundbuffer.loadFromFile("sounds/bop" + std::to_string(i+1) + ".wav"))
        {
            return 1;
        }

        bops[i].setBuffer(soundbuffer);
    }
    
    printControls();

    particleCount = particleCountMax;
    Particle* particleCluster = new Particle[particleCount];
    initCluster(particleCluster, particleCount);

    sf::Event e;

    double t0 = getTime();
    sf::Vector2f windowViewCenter, windowViewSize;
    while (window.isOpen())
    {
        
        while (window.pollEvent(e))
        {
            checkEvent(e, particleCountMax, particleCount);
        }


        window.clear();
        
        double t1 = getTime();

        double tdiff = t1-t0;

        updateCluster(particleCluster, particleCount, tdiff);
        drawCluster(window, particleCluster, particleCount);

        if (SHOWGRAVITYPTS == 1)
        {
            for (size_t i = 0; i < gravPoints.size(); i++)
            {
                window.draw(gravPoints[i].circleIndicator);
            }
        }
        
        
        t0 = t1;


        window.display();
    }

    delete[] particleCluster;
    return 0;
}