#if 1

// ���ڴ�Сȡ����ͼƬ�Ĵ�С��������Ƶ�ͼƬ�ļ���ʱ��ʾ�ķֱ��ʾ���ͼƬ�Ĵ�С


#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

int ts = 54;	// tile size	// ÿ�������ж��ĳ�������ֵ��ȥ�������η���
Vector2i offset(48, 24);	// ƫ������ ���Ͻǵ�λ�õ� ��һ����Ч�����������Ͻǵ�λ��֮��
							// ��windows�Դ��Ļ�ͼ���ߴ�ͼƬ������Ƶ�ͼƬ�ĵ�ʱ�����������½ǿ�����ʱ�������꣬������ƫ����
struct piece
{
	int x, y,/*����ֵ*/ col, row/*�ڼ��У��ڼ���*/, kind/*��ʾ���������͵�С����*/, match/*��ʾ���ڵķ����Ƿ�ƥ��*/, alpha/*͸���ȣ����ڱ�ʾ��������ͬ�ķ�����ʧʱ�Ĺ���*/;
	piece() { match = 0; alpha = 255; kind = -1; }		/* x == col * ts     y == row * ts */
} grid[10][10];		// 8+2���ڱ߽紦��

void swap(piece p1, piece p2)
{
	std::swap(p1.col, p2.col);
	std::swap(p1.row, p2.row);

	grid[p1.row][p1.col] = p1;
	grid[p2.row][p2.col] = p2;
}

int main()
{
	srand(time(0));  // ��Ҫ�����ֵ����

	RenderWindow app(VideoMode(740, 480), "Match-3 Game!");
	app.setFramerateLimit(60);		// ����ˢ�µ����֡��

	Texture t1, t2;		// ����ͼƬ
	t1.loadFromFile("images/background.png");
	t2.loadFromFile("images/gems.png");
	Sprite background(t1), gems(t2);

	// ��ʼ������
	for (int i = 1; i <= 8; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			grid[i][j].kind = rand() % 7;	// �����������Ϊ���ֵ
			grid[i][j].col = j;
			grid[i][j].row = i;
			grid[i][j].x = j * ts;
			grid[i][j].y = i * ts;
		}
		 
	}

	int x0, y0, /* ��һ�ε���ʱ����������λ�ã���¼�к��У�*/x, y; /* �ڶ��ε���ʱ����������λ�ã���¼�к��У�*/
	int click = 0;	//  ����λ�õĵڼ��ε�������2�ε����Ž�������
	Vector2i pos;	// ��굥��ʱ��λ��
	bool isSwap = false /* �Ƿ������� */, isMoving = false /* �Ƿ������ƶ� */;

	while (app.isOpen())
	{
		//----------------------------------------------------------------------------
		// �¼����������û��ĵ���¼�
		Event e;
		while (app.pollEvent(e))
		{
			if (e.type == Event::Closed)
				app.close();

			if (e.type == Event::MouseButtonPressed)
				if (e.key.code == Mouse::Left)
				{
					if (!isSwap && !isMoving) click++;
					pos = Mouse::getPosition(app) /*��ȡ���ڵ����λ��*/ - offset ;
				}
		}

		// mouse click
		if (click == 1)	  // ����ǵ�һ�ε��� 
		{
			x0 = pos.x / ts + 1;	// ������ ת���� ������к��� ����ʾ
			y0 = pos.y / ts + 1;
		}
		if (click == 2)		// ����ǵڶ��ε���
		{
			x = pos.x / ts + 1;
			y = pos.y / ts + 1;
			if (abs(x-x0)+abs(y-y0)==1)		// ������ε����ķ�������
			{
				swap(grid[y0][x0], grid[y][x]);  // �������ڵ�����С����
				isSwap = 1;
				click = 0;
			}
			else
			{
				click = 1;
			}
		}

		//----------------------------------------------------------------------------
		// ���ƥ�����
		// Match finding
		for (int i=1; i<=8; i++)
			for (int j = 1; j <= 8; j++)
			{
				if (grid[i][j].kind == grid[i + 1][j].kind)
					if (grid[i][j].kind == grid[i - 1][j].kind)
						for (int n = -1; n <= 1; n++)
							grid[i + n][j].match++;

				if (grid[i][j].kind == grid[i][j+1].kind)
					if (grid[i][j].kind == grid[i][j-1].kind)
						for (int n = -1; n <= 1; n++)
							grid[i][j+n].match++;
			}


		// �ƶ�����
		// Moving animation
		isMoving = false;
		for (int i = 1; i <= 8; i++)
			for (int j = 1; j <= 8; j++)
			{
				piece &p = grid[i][j];
				int dx, dy;
				for (int n = 0; n < 4; n++)		// 4 - speed	// ÿһ֡�ƶ�4������
				{
					dx = p.x - p.col*ts;
					dy = p.y - p.row*ts;
					if (dx) p.x -= dx / abs(dx);
					if (dy) p.y -= dy / abs(dy);
					if (dx || dy) isMoving = 1;
				}
			}

		// ����
		// Delete amimation
		if (!isMoving)
			for (int i = 1; i <= 8; i++)
				for (int j = 1; j <= 8; j++)
					if (grid[i][j].match)
						if (grid[i][j].alpha>10)
						{
							grid[i][j].alpha -= 10;
							isMoving = true;
						}

		// Get score
		int score = 0;
		for (int i = 1; i <= 8; i++)
			for (int j = 1; j <= 8; j++)
				score += grid[i][j].match;

		// ��ԭ����
		// Second swap if no match
		if (isSwap && !isMoving)
		{
			if (!score)
				swap(grid[y0][x0], grid[y][x]);
			isSwap = 0;
		}

		// ����
		// Update grid
		if (!isMoving)
		{
			for (int i=8; i>0; i--)
				for (int j=1; j<=8; j++)
					if (grid[i][j].match)
						for (int n=i; n>0; n--)
							if (!grid[n][j].match)
							{
								swap(grid[n][j], grid[i][j]);
								break;
							}

			for (int j=1; j<=8; j++)
				for (int i=8, n=0; i>0; i--)
					if (grid[i][j].match)
					{
						grid[i][j].kind = rand() % 7;
						grid[i][j].y = -ts * n++;
						grid[i][j].match = 0;
						grid[i][j].alpha = 255;
					}
		}

		// ��Ⱦ��Ϸ����
		/////////draw//////////
		app.draw(background);	// �Ȼ�����

		for (int i = 1; i <= 8; i++)	// ��Ⱦ���е�С����
			for (int j = 1; j <= 8; j++)
			{
				piece p = grid[i][j];
				gems.setTextureRect(IntRect(p.kind * 49, 0, 49, 49));
				gems.setColor(Color(255, 255, 255, p.alpha));
				gems.setPosition(p.x, p.y);
				gems.move(offset.x - ts, offset.y - ts);
				app.draw(gems);
			}

		// ��ʾ 
		app.display();
	}

	return 0;
}

#endif // 0














#if 0

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
int main(int argc, char const *argv[])
{
	sf::RenderWindow window(sf::VideoMode(400, 400), "Circle");
	window.setFramerateLimit(60);

	sf::CircleShape circle(150);
	circle.setFillColor(sf::Color::Blue);
	circle.setPosition(10, 20);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed
				or (event.type == sf::Event::KeyPressed
					and event.key.code == sf::Keyboard::Escape)) {
				window.close();
			}
			window.clear();
			window.draw(circle);
			window.display();
		}
	}
	return 0;
}

#endif // 0




#if 0




#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "Player.h"

class Game {
public: 
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game();
	void run(int minimum_frame_per_seconds);
private:
	void processEvents();
	void update(sf::Time deltatime);
	void render();
	sf::RenderWindow _window;
	Player _player;
};

Game::Game() : _window(sf::VideoMode(800, 600), "Game") { }

void Game::run(int minimum_frame_per_seconds) {
	sf::Clock clock;
	sf::Time timeSinceLastUpdate;
	sf::Time TimePerFrame = sf::seconds(1.f / minimum_frame_per_seconds);

	while (_window.isOpen()) {
		processEvents();
		timeSinceLastUpdate = clock.restart();

		while (timeSinceLastUpdate > TimePerFrame) {
			timeSinceLastUpdate -= TimePerFrame;
			update(TimePerFrame);
		}
		update(timeSinceLastUpdate);
		render();
	}
}

void Game::processEvents() {
	sf::Event event;
	while (_window.pollEvent(event)) {
		if ((event.type == sf::Event::Closed) ) {
			_window.close();
		}
		else if ((event.type == sf::Event::KeyPressed)) {
			if (event.key.code == sf::Keyboard::Escape) {
				_window.close();
			} else if (event.key.code == sf::Keyboard::Up) {
				_player.isMoving = true;
			}
			else if (event.key.code == sf::Keyboard::Left) {
				_player.rotation = -1;
			}
			else if (event.key.code == sf::Keyboard::Right) {
				_player.rotation = 1;
			}
		}
		else if (event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::Up) 
				_player.isMoving = false;
			else if (event.key.code == sf::Keyboard::Left)
				_player.rotation = 0;
			else if (event.key.code == sf::Keyboard::Right)
				_player.rotation = 0;
		}
	}
}

void Game::update(sf::Time deltatime) { 
	_player.update(deltatime);
}

void Game::render() {
	_window.clear();
	_window.draw(_player);
	_window.display();
}

//class Player : public sf::Drawable {
//public:
//
//};

int main(int argc, char* argv[])
{
	Game game;
	game.run(60);



	return 0;
}

#endif // 0