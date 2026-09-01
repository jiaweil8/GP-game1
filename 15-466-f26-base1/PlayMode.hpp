#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;
	
	void reset_game();

	//----- game state -----

	//input tracking:
	struct Button {
    	uint8_t downs = 0;
    	uint8_t pressed = 0;
	} left, right;

	//paddle:
	glm::vec2 paddle_at = glm::vec2(104.0f, 16.0f);
	uint8_t paddle_tiles = 6;

	//ball:
	glm::vec2 ball_at = glm::vec2(124.0f, 80.0f);
	glm::vec2 ball_velocity = glm::vec2(70.0f, 90.0f);

	//bricks:
	uint32_t total_bricks = 72;
	uint32_t remaining_bricks = 72;

	//game status:
	bool game_over = false;
	bool won = false;
	float win_flash_timer = 0.0f;

	//----- drawing handled by PPU466 -----
	PPU466 ppu;
};
