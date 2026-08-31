#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

//asset pipeline generated files
#include "generated/assets.hpp"

#include <random>

PlayMode::PlayMode() {

    //tiles:
    ppu.tile_table[0] = Assets::empty;
    ppu.tile_table[1] = Assets::paddle;
    ppu.tile_table[2] = Assets::ball;
    ppu.tile_table[3] = Assets::brick;

    //palettes:
    ppu.palette_table[0] = Assets::paddle_palette;
    ppu.palette_table[1] = Assets::ball_palette;
    ppu.palette_table[2] = Assets::brick_palette;

    //empty background:
    for (auto &cell : ppu.background) {
        cell = 0;
    }
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	 if (evt.type == SDL_EVENT_KEY_DOWN) {
        if (evt.key.key == SDLK_LEFT) {
            left.downs += 1;
            left.pressed = true;
            return true;
        } else if (evt.key.key == SDLK_RIGHT) {
            right.downs += 1;
            right.pressed = true;
            return true;
        }
    } else if (evt.type == SDL_EVENT_KEY_UP) {
        if (evt.key.key == SDLK_LEFT) {
            left.pressed = false;
            return true;
        } else if (evt.key.key == SDLK_RIGHT) {
            right.pressed = false;
            return true;
        }
    }
	return false;
}

void PlayMode::update(float elapsed) {

    constexpr float PaddleSpeed = 100.0f;

    if (left.pressed) {
        paddle_at.x -= PaddleSpeed * elapsed;
    }
    if (right.pressed) {
        paddle_at.x += PaddleSpeed * elapsed;
    }

    //keep paddle on screen:
    if (paddle_at.x < 0.0f) {
        paddle_at.x = 0.0f;
    }
    if (paddle_at.x > 224.0f) {
        paddle_at.x = 224.0f;
    }

    //reset button press counters:
    left.downs = 0;
    right.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {

    ppu.background_color =
        glm::u8vec3(0x10, 0x10, 0x20);

    //hide all sprites first:
    for (auto &sprite : ppu.sprites) {
        sprite.y = 240;
    }

    //draw paddle:
    for (uint8_t i = 0; i < 4; ++i) {

        ppu.sprites[i].x =
            uint8_t(paddle_at.x) + 8 * i;

        ppu.sprites[i].y =
            uint8_t(paddle_at.y);

        ppu.sprites[i].index = 1;
        ppu.sprites[i].attributes = 0;
    }

    //draw ball:
    ppu.sprites[4].x =
        uint8_t(ball_at.x);

    ppu.sprites[4].y =
        uint8_t(ball_at.y);

    ppu.sprites[4].index = 2;
    ppu.sprites[4].attributes = 1;

    ppu.draw(drawable_size);
}
