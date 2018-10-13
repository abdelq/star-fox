#include "tp1.h"
#include <glm/gtx/string_cast.hpp>

CoreTP1::CoreTP1() : Core(), floor(1, vec4(135.0/255, 206.0/255, 250.0/255, 0.75)), f(0), sky(2, vec4(0.0, 0.0, 1.0, 0.5))
{
	// Initialize view matrix
	_viewMatrix = lookAt(vec3(0, 0, 20), vec3(0, 0, 0), vec3(0, 1, 0));

	// Rescale sky
	sky.SetTransform(scale(mat4(), vec3(110.0f)));

}

void CoreTP1::Render(double dt)
{

	if (!game_over)
	{

		// Clean projectiles and fighters if needed
		clean_scene();

		// Make a fighter appear if needed
		spawn_enemies();

		// Make existing fighters fire
		fire_enemies();

		// Update player properties (acceleration, speed, orientation, ...)
		player.Update(dt);
		
		auto time = glfwGetTime();
		if (player.input[Player::Input::SPACE] && time - player.last_shot > player.shot_delay)
		{
			player.last_shot = time;
			for (vec3 point : player.getProjectileSpawnPoint())
			{
				active_projectiles.push_back(std::make_unique<Projectile>(
					point, player.projectile_speed,
					player.projectile_color_in, player.projectile_color_out,
					true
				));
			}
		}

		// Transform and display floor
		f += float(dt) * 2 * pi<float>() * 0.1f;
		floor.SetTransform(translate(mat4(), vec3(0.0f, -13.0f, 0.0f)) *scale(mat4(), vec3(100.0f, 1.0f, 100.0f)) * rotate(mat4(), -1.0f * f, vec3(1.0f, 0.0f, 0.0f)));
		floor.Render();

		// Display sky
		sky.Render();

		// Make the ship winking during the "peaceful period"
		if (time - start_time > spawn_delay_after_start || time - start_time < 1.0 || time - start_time > 1.5 && time - start_time < 2.0 || time - start_time > 2.5 && time - start_time < 3.0 || time - start_time > 3.5 && time - start_time < 4.0 || time - start_time > 4.5 && time - start_time < 5.0)
			player.Render();

		bool player_shot = false;

		for (auto proj = active_projectiles.begin(); proj != active_projectiles.end();)
		{
			(*proj)->Update(dt);

			bool hit_something = false;

			// If the projectile we consider comes from an enemy
			if (!(*proj)->friendly())
			{
				hit_something = player.Intersect((*proj)->position());
				player_shot |= hit_something;
			}
			// If the projectile we consider comes from the player
			else
			{
				for (auto fighter = active_fighters.begin(); fighter != active_fighters.end();)
				{
					if ((*fighter)->Intersect((*proj)->position()))
					{
						hit_something = true;
						player.score += (*fighter)->score;
						fighter = active_fighters.erase(fighter);
					}
					else
					{
					    ++fighter;
					}
				}
			}

			// If the projectile has hit something
			if (hit_something)
			{
				proj = active_projectiles.erase(proj);
			}
			else
			{
				(*proj)->Render();
				++proj;
			}
		}

		// If the player has been shot
		if (player_shot)
		{
			player_hit();
		}

		for (auto fighter = active_fighters.begin(); fighter != active_fighters.end(); ++fighter)
		{
			(*fighter)->Update(dt);
			(*fighter)->Render();
		}


		if (display_aabb)
		{
			// Clear AABB lines buffer
			_lineVertices.clear();

			// Fill AABB lines for player
			auto globalAABB = player.GetGlobalAABB();
			AABB(globalAABB.min, globalAABB.max);
			for (auto box : player.GetAABB())
				AABB(box.min, box.max);

			// Fill AABB lines for fighters
			for (auto& fighter : active_fighters)
			{
				auto globalAABB = fighter->GetGlobalAABB();
				AABB(globalAABB.min, globalAABB.max);
				for (auto box : fighter->GetAABB())
					AABB(box.min, box.max);
			}

			DrawAABBs();
		}


		if (player.god_mode)
		{
			DrawText("INVICIBLE !", vec2(0.5, 0), vec4(1, 1, 0, 1), 16U, ALIGN_CENTER);
		}

		// Display player lifes and score
		DrawGameText(player.lifes, player.score);
	}
	else
	{
		// Draw score when game_over
		DrawEndGameText(player.score);
	}
}

CoreTP1::~CoreTP1()
{
}

void CoreTP1::OnKeyW(bool down)
{
	if (!game_over)
	{
		player.input[Player::Input::UP] = down;
	}
}

void CoreTP1::OnKeyS(bool down)
{
	if (!game_over)
	{
		player.input[Player::Input::DOWN] = down;

	}
}

void CoreTP1::OnKeyA(bool down)
{
	if (!game_over)
	{
		player.input[Player::Input::LEFT] = down;
	}
}

void CoreTP1::OnKeyD(bool down)
{
	if (!game_over)
	{
		player.input[Player::Input::RIGHT] = down;
	}
}

void CoreTP1::OnKeyE(bool down)
{
	if (down && !game_over)
	{
		display_aabb = !display_aabb;
	}
}

void CoreTP1::OnKeySPACE(bool down)
{
	if (!game_over)
	{
		player.input[Player::Input::SPACE] = down;
	}
	else
	{
		game_over = false;
		start_time = glfwGetTime();
		player = Player();
	}
}

void CoreTP1::OnKeyTAB(bool down)
{
	if (down)
	{
		player.god_mode = !player.god_mode;
	}
}

void CoreTP1::DrawEndGameText(int score)
{
	DrawText("Fin de partie!", vec2(0.5, 0.6), vec4(1), 32U, ALIGN_CENTER);
	DrawText((std::string("Pointage: ") + std::to_string(score)).c_str(), vec2(0.5, 0.5), vec4(1), 32U, ALIGN_CENTER);
}

void CoreTP1::DrawGameText(int lives, int score)
{
	DrawText((std::string("Vies: ") + std::to_string(lives)).c_str(), vec2(0.99, 0.01), vec4(1), 16U, ALIGN_RIGHT);
	DrawText((std::string("Pointage: ") + std::to_string(score)).c_str(), vec2(0.01, 0.01), vec4(1), 16U, ALIGN_LEFT);
}

void CoreTP1::spawn_enemies()
{
	auto time = glfwGetTime();
	if (time - start_time > spawn_delay_after_start && time - last_spawn > spawn_delay)
	{
		last_spawn = time;

		vec3 spawn = vec3(rand() % 17 - 8, rand() % 15 - 7, -100);
		if (rand() % 100 < 75)
		{
			active_fighters.push_back(std::make_unique<Fighter1>(
				spawn, vec3(0, 0, 2.5), 2, vec3(0, 0, 5)
			));
		}
		else
		{
			active_fighters.push_back(std::make_unique<Fighter2>(
				spawn, vec3(0, 0, 5), 4, vec3(0, 0, 10)
			));
		}
	}
}

void CoreTP1::fire_enemies()
{
	for (auto& fighter : active_fighters)
	{
		auto time = glfwGetTime();
		if (time - fighter->last_shot > fighter->rof)
		{
			fighter->last_shot = time;
			for (vec3 point : fighter->GetProjectileSpawnPoint())
			{
				// TODO Schema de tir pour le 2e ennemi, maybe on each update for the second put projectile_vel as player pos ?
				active_projectiles.push_back(std::make_unique<Projectile>(
					point, fighter->projectile_vel,
					vec4(0.0f, 0.0f, 0.7f, 1.0f), vec4(0.0f, 0.0f, 0.7f, 0.8f),
					false
				));
			}
		}
	}
}

void CoreTP1::clean_scene()
{
	for (auto proj = active_projectiles.begin(); proj != active_projectiles.end();)
	{
		if ((*proj)->position().z > 10 || (*proj)->position().z < -100)
			proj = active_projectiles.erase(proj);
		else
			++proj;
	}

	for (auto fighter = active_fighters.begin(); fighter != active_fighters.end();)
	{
		if ((*fighter)->Position.z > 5)
		{
			player.score -= (*fighter)->score;
			fighter = active_fighters.erase(fighter);
		}
		else
		{
			++fighter;
		}
	}
}

void CoreTP1::clear_scene()
{
	clean_scene();
	start_time = glfwGetTime();
	player = Player();
}

void CoreTP1::player_hit()
{
	if (!player.god_mode)
	{
		start_time = glfwGetTime();

		active_fighters.clear();
		active_projectiles.clear();

		player.Position = vec3(0);
		if (--player.lifes == 0)
			game_over = true;
	}
}
