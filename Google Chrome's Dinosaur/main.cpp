#define OLC_PGE_APPLICATION
#include "Header Files/olcPixelGameEngine.h"

class Dinosaur : public olc::PixelGameEngine {
private:
	struct Box {
		float x, y, w, h;
	};
public:
	Dinosaur() {
		sAppName = "Google Chrome's Dinosaur";
	}

public:
	bool OnUserCreate() override {

		SetSprites();
		SetPlayer();

		// tiles
		for (int x = 0; x < 25; x++) {
			tilePos.push_back({ (float)x * 50.0f, 550.0f });
		}
		// cactus
		for (int i = 2; i <= 4; i++) {
			if (i % 2 == 1) {
				this->sprite = new olc::Sprite("./Dependencies/Objects/c2.png");
			}
			else {
				this->sprite = new olc::Sprite("./Dependencies/Objects/c3.png");
			}
			olc::Decal* decal = new olc::Decal(sprite);
			float x = (((float)ScreenWidth() / 2 + ScreenWidth() * i - 1) + (ScreenWidth() / 4));
			float y = (float)ScreenHeight() - 150.0f;
			olc::vf2d pos = { x, y };
			cactuses.push_back(std::make_pair(pos, decal));
		}
		
		// objects
		olc::vf2d pos = {};
		pos = { 650.0f , 500.0f };
		this->sprite = new olc::Sprite("./Dependencies/Objects/Stone.png");
		objects.push_back(std::make_pair(pos, new olc::Decal(sprite)));
		pos = { 600.0f , 500.0f};
		this->sprite = new olc::Sprite("./Dependencies/Objects/Skeleton.png");
		objects.push_back(std::make_pair(pos, new olc::Decal(sprite)));
		pos = { 400.0f , 460.0f };
		this->sprite = new olc::Sprite("./Dependencies/Objects/SignArrow.png");
		objects.push_back(std::make_pair(pos, new olc::Decal(sprite)));

		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override { 
		Clear(olc::BLANK);
		
		if (dawn) {
			DrawDecal({ 0.0f, 0.0f }, decalBG2, { 1.0f, 1.0f }, light);
		}
		else {
			DrawDecal({ 0.0f, 0.0f }, decalBG1, { 1.0f, 1.0f }, light);
		}
		DrawDecal(moonPos, moon, { 1.0f, 1.0f }, light);
		DrawDecal(sunPos, sun, { 1.0f, 1.0f }, light);
		DrawStringDecal({ 0.0, 2.0f }, "SCORE: " + std::to_string(score), olc::WHITE, { 2.0f, 2.0f });

		for (auto o : objects) {
			DrawDecal(o.first, o.second, { 0.5f, 0.5f }, light);
		}

		Jump();
		Animate();
		MoveCactus();
		MoveTiles();
		DayToNightShift();

		if (multiplier <= 4.0f) {
			multiplier += 0.001f;
		}
		else if (multiplier <= 6.0f) {
			multiplier += 0.0001f;
		}
		else if (multiplier <= 8.0f) {
			multiplier += 0.00001f;
		}
		else {
			multiplier += 0.0000001f;
		}

		return true;
	}

	bool Collision(Box box1, Box box2) {
		// TODO : collision : DONE
		int c = 0;
		if (box1.x < box2.x + box2.w) c++;
		if (box1.x + box1.w > box2.x) c++;
		if (box1.y < box2.y + box2.h) c++;
		if (box1.h + box1.y > box2.y) c++;
		
		if (c == 4) {
			return true;
		}
		return false;
	}
	void Jump() {
		// TODO : jumping : DONE
		if (landing && (GetKey(olc::Key::SPACE).bPressed || GetKey(olc::Key::UP).bPressed)) {
			jump = true;
			landing = false;
		}
		if (!landing && jump) {
			for (auto& pos : vertices) {
				pos.y -= jumpForce;
			}
			jumpCount--;
			if (jumpCount <= 0) {
				jumpCount = 150;
				jump = false;
			}
		}
	}
	void Gravity() {
		// TODO : gravity : DONE
		for (auto& pos : vertices) {
			pos.y += gravity;
		}
	}
	void Animate() {
		if (index >= dinosaurSprites.size()) {
			index = 0;
		}

		DrawWarpedDecal(dinosaurSprites[index], vertices, light);
		if (delay > 10) {
			score++;
			index++;
			delay = 0;
		}
			delay++;
	}
	void SetPlayer() {
		speed = 0.4f;
		width = 125.0f; height = 125.0f;
		x = 100.0f; y = 100.0f;
		r = 250; g = 250; b = 250;
		index = 0; delay = 0; score = 0; counter = 0; tileIndex = 0;
		gravity = 1.81f; jumpCount = 150; jumpForce = gravity * 2.0f; multiplier = 2.0f;
		dawn = false; jump = false; landing = false; ifDay = false;

		vertices[0] = { x,		 y };
		vertices[1] = { x,		 y + height};
		vertices[2] = { x + width, y + height };
		vertices[3] = { x + width, y };
	}
	void MoveTiles() {
		bool collide = false;
		for (auto& pos : tilePos) {
			// draw tiles
			pos.x -= speed * multiplier;
			DrawDecal(pos, tiles, { 0.5f, 0.5f }, light);

			// check collision
			auto w = tiles->sprite->width * 0.5f;
			auto h = tiles->sprite->height * 0.5f;
			Box box1 = { pos.x, pos.y, w, h };
			Box box2 = { vertices[0].x, vertices[0].y, width, height - 5 };
			if (Collision(box1, box2)) {
				landing = true;
				collide = true;
			}
			if (tileIndex >= tilePos.size()) {
				tileIndex = 0;
			}
			if (tilePos[tileIndex].x <= -50) {
				unsigned int tail = 0;
				if (tileIndex == 0) {
					tail = tilePos.size() - 1;
				}
				else {
					tail = tileIndex - 1;
				}
				tilePos[tileIndex].x = tilePos[tail].x + w;
				tileIndex++;
			}
		}
		if (!collide) {
			Gravity();
		}
	}
	void MoveCactus() {
		;
		for (auto& cactus : cactuses) {
			if (cactus.first.x <= -50) {
				if (counter % 2 == 0) {
					cactus.first.x = (float)ScreenWidth() * 2;
				}
				else {
					cactus.first.x = (float)ScreenWidth() * 3;
				}
			}
			cactus.first.x -= speed * multiplier;

			DrawDecal(cactus.first, cactus.second, { 1.0f, 1.0f }, light);
			auto w = (float)cactus.second->sprite->width;
			auto h = (float)cactus.second->sprite->height;
			DrawRect({ (int)cactus.first.x, (int)cactus.first.y }, { (int)w, (int)h }, olc::YELLOW);
			Box box1 = { vertices[0].x, vertices[0].y, width - 40, height - 10 };
			Box box2 = { cactus.first.x, cactus.first.y, w, h };

			if (Collision(box1, box2)) {
				score = 0;
				return;
			}
		}
	}
	void SetSprites() {
		this->sprite = new olc::Sprite("./Dependencies/BG/BG.png");
		this->decalBG1 = new olc::Decal(sprite);
		this->sprite = new olc::Sprite("./Dependencies/BG/BG1.png");
		this->decalBG2 = new olc::Decal(sprite);
		this->sprite = new olc::Sprite("./Dependencies/Objects/sun.png");
		this->sun = new olc::Decal(sprite);
		this->sprite = new olc::Sprite("./Dependencies/Objects/moon.png");
		this->moon = new olc::Decal(sprite);
		this->light = olc::Pixel(r, g, b, 250);
		
		starX = (float)ScreenWidth() - sun->sprite->width;
		starY = -50.0f;
		sunPos = { starX, starY };
		moonPos = { starX + 150, (float)-moon->sprite->height };

		// tiles
		this->sprite = new olc::Sprite("./Dependencies/Objects/StoneBlock.png");
		this->tiles = new olc::Decal(sprite);

		for (int i = 1; i <= 8; i++) {
			this->sprite = new olc::Sprite("./Dependencies/Dinosaur/Run (" + std::to_string(i) + ").png");
			dinosaurSprites.push_back(new olc::Decal(this->sprite));
		}
	}
	void DayToNightShift() {
		// TODO : day and night : DONE
		this->light = olc::Pixel(r, g, b, 250);
		if (counter == 4000) {
			counter = 0;
			if (!dawn) {
				dawn = true;
			}
			else {
				dawn = false;
			}
		}
		if (!dawn) {
			if (r < 250) {
				r += 1;
				g += 1;
				b += 1;
			}
			if (moonPos.x < ScreenWidth() && ifDay) {
				moonPos.x += 2.0f;
				moonPos.y += 1.0f;
				if (moonPos.x >= ScreenWidth()) {
					moonPos.y = (float)-moon->sprite->height;
					moonPos.x = starX + 150;
					
					ifDay = false;
				}
			}
			if (sunPos.y < starY) {
				sunPos.y += 1.0f;
			}
		}
		else if (dawn) {
			if (r > 100) {
				r -= 1;
				g -= 1;
				b -= 1;
			}
			if (sunPos.x < ScreenWidth() && !ifDay) {
				sunPos.x += 2.0f;
				sunPos.y += 1.0f;
				if (sunPos.x >= ScreenWidth()) {
					sunPos.y = (float)-sun->sprite->height;
					sunPos.x = starX;
					
					ifDay = true;
				}
			}
			if (moonPos.y < (starY + 150)) {
				moonPos.y += 1.0f;
			}
		}
		counter++;
	}


private:
	int r, g, b;
	bool dawn, jump, landing, ifDay;
	long long counter;
	unsigned int index, tileIndex;
	unsigned int delay;
	unsigned long long score;
	float x, y, width, height, speed, gravity, jumpForce, jumpCount, multiplier, starX, starY;
	olc::Pixel light;
	olc::Decal* sun;
	olc::Decal* moon;
	olc::Decal* tiles;
	olc::Decal* decalBG1;
	olc::Decal* decalBG2;
	olc::Sprite* sprite;
	olc::vf2d moonPos = {};
	olc::vf2d sunPos = {};
	std::array<olc::vf2d, 4> vertices;
	std::vector<olc::vf2d> tilePos;
	std::vector<olc::Decal*> dinosaurSprites;
	std::vector<std::pair<olc::vf2d, olc::Decal*>> cactuses;
	std::vector<std::pair<olc::vf2d, olc::Decal*>> objects;
};

int main() {

	Dinosaur dinosaur;
	if (dinosaur.Construct(1100, 600, 1, 1))
		dinosaur.Start();

	return 0;
}