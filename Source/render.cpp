#include "render.h"

void RenderStartScreen()
{
	DrawText("SPACE INVADERS", 200, 100, 160, YELLOW);
	DrawText("PRESS SPACE TO BEGIN", 200, 350, 40, YELLOW);
}

void RenderGameplay(Background& background, int score, Resources& resources, 
	std::vector<Projectile>& projectiles, std::vector<Wall>& walls, std::vector<Alien>& aliens, Player& player){
	// background render LEAVE THIS AT TOP
	background.Render();

	DrawText(TextFormat("Score: %i", score), 50, 20, 40, YELLOW);
	DrawText(TextFormat("Lives: %i", player.lives), 50, 70, 40, YELLOW);

	// player rendering
	player.Render(resources.shipTextures[player.activeTexture]);

	// Use range-based for loops for rendering
	for (const auto& projectile : projectiles)
	{
		projectile.Render(resources.laserTexture);
	}

	for (const auto& wall : walls)
	{
		wall.Render(resources.barrierTexture);
	}

	for (const auto& alien : aliens)
	{
		alien.Render(resources.alienTexture);
	}
}

void RenderEndScreen(bool newHighScore, TextInputState textInput,std::vector<PlayerData> leaderboard)
{
	if (newHighScore)
	{
		RenderNewHighScore(textInput);
	}
	else
	{
		RenderLeaderboard(leaderboard);
	}
}

void RenderNewHighScore(TextInputState textInput)
{
	DrawText("NEW HIGHSCORE!", 600, 300, 60, YELLOW);
	DrawText("PLACE MOUSE OVER INPUT BOX!", 600, 400, 20, YELLOW);

	DrawRectangleRec(textInput.textBox, LIGHTGRAY);

	const Color borderColor = textInput.mouseOnText ? RED : DARKGRAY;
	DrawRectangleLines(
		static_cast<int>(textInput.textBox.x),
		static_cast<int>(textInput.textBox.y),
		static_cast<int>(textInput.textBox.width),
		static_cast<int>(textInput.textBox.height),
		borderColor
	);

	DrawText(textInput.name.c_str(), static_cast<int>(textInput.textBox.x) + 5, static_cast<int>(textInput.textBox.y) + 8, 40, MAROON);
	DrawText(TextFormat("INPUT CHARS: %i/%i", static_cast<int>(textInput.name.length()), textInput.maxNameLength), 600, 600, 20, YELLOW);

	if (textInput.mouseOnText)
	{
		if (textInput.name.length() <= textInput.maxNameLength)
		{
			// Draw blinking underscore char
			if (((textInput.framesCounter / 20) % 2) == 0)
			{
				DrawText("_",
					static_cast<int>(textInput.textBox.x) + 8 + MeasureText(textInput.name.c_str(), 40),
					static_cast<int>(textInput.textBox.y) + 12,
					40,
					MAROON
				);
			}
		}
		else
		{
			DrawText("Press BACKSPACE to delete chars...", 600, 650, 20, YELLOW);
		}
	}

	if (!textInput.name.empty() && textInput.name.length() < 9)
	{
		DrawText("PRESS ENTER TO CONTINUE", 600, 800, 40, YELLOW);
	}
}

void RenderLeaderboard(std::vector<PlayerData> leaderboard)
{
	DrawText("PRESS ENTER TO CONTINUE", 600, 200, 40, YELLOW);
	DrawText("LEADERBOARD", 50, 100, 40, YELLOW);

	for (size_t i = 0; i < leaderboard.size(); ++i)
	{
		const int yPos = 140 + static_cast<int>(i * 40);
		DrawText(leaderboard[i].name.c_str(), 50, yPos, 40, YELLOW);
		DrawText(TextFormat("%i", leaderboard[i].score), 350, yPos, 40, YELLOW);
	}
}