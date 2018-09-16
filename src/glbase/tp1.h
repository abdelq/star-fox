#include <main.h>
#include "core.h"
#include "scene.h"
#include "player.h"

class CoreTP1 : public Core
{
public:
	CoreTP1();
	virtual ~CoreTP1() override;

protected:
	virtual void Render(double dt) override;
	virtual void OnKeyW(bool down) override;
	virtual void OnKeyS(bool down) override;
	virtual void OnKeyA(bool down) override;
	virtual void OnKeyD(bool down) override;
	virtual void OnKeyE(bool down) override;
	virtual void OnKeySPACE(bool down) override;
	virtual void OnKeyTAB(bool down) override;
	void DrawEndGameText(int score);
	void DrawGameText(int lives, int score);

	void spawn_enemies();
	void fire_enemies();
	void clean_scene();

	std::vector<std::unique_ptr<Projectile>> active_projectiles;
	std::vector<std::unique_ptr<Entity>> active_fighters;

protected:
	Player player;
	Sphere floor;
	Sphere sky;

	void clear_scene();
	void player_hit();

	float f;

	bool game_over = false;
	bool display_aabb = false;

	double start_time = 0.0;
	double spawn_delay_after_start = 5.0;
	double spawn_delay = 3.0;

	double last_spawn = 0.0;
};