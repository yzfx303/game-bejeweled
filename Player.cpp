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

void Player::update(sf::Time deltaTime)
{
	float seconds = deltaTime.asSeconds();

	if (rotation != 0) {
		float angle = (rotation > 0 ? 1 : -1) * 180 * seconds;
		_shape.rotate(angle);
	}

	if (isMoving) {
		float angle = _shape.getRotation() / 180 * M_PI - M_PI / 2;
		_velocity += sf::Vector2f(std::cos(angle), std::sin(angle))*60.f*seconds;
	}
	_shape.move(seconds*_velocity);
}

void Player::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
}



