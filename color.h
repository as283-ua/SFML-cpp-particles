#include <SFML/Graphics.hpp>

sf::Color nextcolor(sf::Color color){
    int increment = 1; //dividend of 255

    while (color.r%increment != 0)
    {
        color.r++;
    }

    while (color.g%increment != 0)
    {
        color.g++;
    }

    while (color.b%increment != 0)
    {
        color.b++;
    }
    
    if (color.r==255 && color.g < 255 && color.b<1)
    {
        color.g+=increment;
    } else if (color.g==255 && color.r > 0)
    {
        color.r-=increment;
    } else if (color.g==255 && color.b < 255)
    {
        color.b+=increment;
    } else if (color.b==255 && color.g > 0)
    {
        color.g-=increment;
    } else if (color.b==255 && color.r < 255)
    {
        color.r+=increment;
    } else if (color.r==255 && color.b > 0)
    {
        color.b-=increment;
    } else {
        color.r+=increment;
    }

    return color;
}

sf::Color randomColor(int seed){srand(seed);return sf::Color(rand()%256,rand()%256,rand()%256,255);}