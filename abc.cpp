#include "Player.h"
//#include <cmath>
#include <corecrt_math_defines.h>

//const float M_PI = 3.14159;


Player::Player() : _shape(sf::Vector2f(100,100))
{
	_shape.setFillColor(sf::Color::Blue);
	_shape.setOrigin(50, 50);
	_shape.setPosition(400, 300);
}
