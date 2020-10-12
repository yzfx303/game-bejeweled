#if 1

// 窗口大小取背景图片的大小，把鼠标移到图片文件上时显示的分辨率就是图片的大小


#include <SFML/Graphics.hpp>
#include <time.h>
using namespace sf;

int ts = 54;	// tile size	// 每个方块有多大的长，像素值，去量正方形方框
Vector2i offset(48, 24);	// 偏移量， 左上角的位置到 第一个有效的正方形左上角的位置之差
							// 用windows自带的画图工具打开图片，鼠标移到图片的点时，可以在左下角看到此时鼠标的坐标，可用于偏移量
struct piece
{
	int x, y,/*坐标值*/ col, row/*第几行，第几列*/, kind/*表示是哪种类型的小方块*/, match/*表示相邻的方块是否匹配*/, alpha/*透明度，用于表示当三个相同的方块消失时的过程*/;
	piece() { match = 0; alpha = 255; kind = -1; }		/* x == col * ts     y == row * ts */
} grid[10][10];		// 8+2便于边界处理

void swap(piece p1, piece p2)
{
	std::swap(p1.col, p2.col);
	std::swap(p1.row, p2.row);

	grid[p1.row][p1.col] = p1;
	grid[p2.row][p2.col] = p2;
}

int main()
{
	srand(time(0));  // 需要设随机值种子

	RenderWindow app(VideoMode(740, 480), "Match-3 Game!");
	app.setFramerateLimit(60);		// 设置刷新的最大帧率

	Texture t1, t2;		// 加载图片
	t1.loadFromFile("images/background.png");
	t2.loadFromFile("images/gems.png");
	Sprite background(t1), gems(t2);

	// 初始化方块
	for (int i = 1; i <= 8; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			grid[i][j].kind = rand() % 7;	// 方块的种类设为随机值
			grid[i][j].col = j;
			grid[i][j].row = i;
			grid[i][j].x = j * ts;
			grid[i][j].y = i * ts;
		}
		 
	}

	int x0, y0, /* 第一次单击时的鼠标的坐标位置（记录行和列）*/x, y; /* 第二次单击时的鼠标的坐标位置（记录行和列）*/
	int click = 0;	//  相邻位置的第几次单击，第2次单击才交换方块
	Vector2i pos;	// 鼠标单击时的位置
	bool isSwap = false /* 是否发生交换 */, isMoving = false /* 是否正在移动 */;

	while (app.isOpen())
	{
		//----------------------------------------------------------------------------
		// 事件处理，处理用户的点击事件
		Event e;
		while (app.pollEvent(e))
		{
			if (e.type == Event::Closed)
				app.close();

			if (e.type == Event::MouseButtonPressed)
				if (e.key.code == Mouse::Left)
				{
					if (!isSwap && !isMoving) click++;
					pos = Mouse::getPosition(app) /*获取窗口的鼠标位置*/ - offset ;
				}
		}

		// mouse click
		if (click == 1)	  // 如果是第一次单击 
		{
			x0 = pos.x / ts + 1;	// 把像素 转化成 方块的行和列 来表示
			y0 = pos.y / ts + 1;
		}
		if (click == 2)		// 如果是第二次单击
		{
			x = pos.x / ts + 1;
			y = pos.y / ts + 1;
			if (abs(x-x0)+abs(y-y0)==1)		// 如果两次单击的方块相邻
			{
				swap(grid[y0][x0], grid[y][x]);  // 交换相邻的两个小方块
				isSwap = 1;
				click = 0;
			}
			else
			{
				click = 1;
			}
		}

		//----------------------------------------------------------------------------
		// 检查匹配情况
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


		// 移动处理
		// Moving animation
		isMoving = false;
		for (int i = 1; i <= 8; i++)
			for (int j = 1; j <= 8; j++)
			{
				piece &p = grid[i][j];
				int dx, dy;
				for (int n = 0; n < 4; n++)		// 4 - speed	// 每一帧移动4个像素
				{
					dx = p.x - p.col*ts;
					dy = p.y - p.row*ts;
					if (dx) p.x -= dx / abs(dx);
					if (dy) p.y -= dy / abs(dy);
					if (dx || dy) isMoving = 1;
				}
			}

		// 消除
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

		// 还原处理
		// Second swap if no match
		if (isSwap && !isMoving)
		{
			if (!score)
				swap(grid[y0][x0], grid[y][x]);
			isSwap = 0;
		}

		// 更新
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

		// 渲染游戏画面
		/////////draw//////////
		app.draw(background);	// 先画背景

		for (int i = 1; i <= 8; i++)	// 渲染所有的小方块
			for (int j = 1; j <= 8; j++)
			{
				piece p = grid[i][j];
				gems.setTextureRect(IntRect(p.kind * 49, 0, 49, 49));
				gems.setColor(Color(255, 255, 255, p.alpha));
				gems.setPosition(p.x, p.y);
				gems.move(offset.x - ts, offset.y - ts);
				app.draw(gems);
			}

		// 显示 
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