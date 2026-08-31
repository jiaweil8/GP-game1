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

    //corresponds to tile 3, palette 2 (brick):
    uint16_t brick_cell = 3 | (2 << 8);

    for (uint32_t y = 20; y < 25; y+=2) {
        for (uint32_t x = 4; x < 28; ++x) {
            ppu.background[
                x + y * PPU466::BackgroundWidth
            ] = brick_cell;
        }
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
    if (paddle_at.x > 256-8.0f*paddle_tiles) {
        paddle_at.x = 256-8.0f*paddle_tiles;
    }
    
    //ball movement:
    ball_at += ball_velocity * elapsed;
    //bounce off walls:
    if (ball_at.x < 0.0f) {
        ball_at.x = 0.0f;
        ball_velocity.x = -ball_velocity.x;
    }
    if (ball_at.x > 256 - 8.0f) {
        ball_at.x = 256 - 8.0f;
        ball_velocity.x = -ball_velocity.x;
    }
    if (ball_at.y > 240 - 8.0f) {
        ball_at.y = 240 - 8.0f;
        ball_velocity.y = -ball_velocity.y;
    }
    
    float ball_left = ball_at.x;
    float ball_right = ball_at.x + 8.0f;
    float ball_bottom = ball_at.y;
    float ball_top = ball_at.y + 8.0f;

    //bounce off paddle:
    float paddle_left = paddle_at.x;
    float paddle_right = paddle_at.x + 8.0f * paddle_tiles;
    float paddle_bottom = paddle_at.y;
    float paddle_top = paddle_at.y + 8.0f;
    bool overlaps_paddle =
        ball_right > paddle_left &&
        ball_left < paddle_right &&
        ball_top > paddle_bottom &&
        ball_bottom < paddle_top;
    //if ball is moving down and overlaps the top of the paddle, bounce it:
    if (ball_velocity.y < 0.0f && overlaps_paddle) {
        ball_at.y = paddle_top;
        ball_velocity.y = -ball_velocity.y;
    }

    //bounce off bricks:
    int32_t left_tile   = int32_t(ball_at.x) / 8;
    int32_t right_tile  = int32_t(ball_at.x + 7.0f) / 8;
    int32_t bottom_tile = int32_t(ball_at.y) / 8;
    int32_t top_tile    = int32_t(ball_at.y + 7.0f) / 8;
    bool hit_brick = false;
    for (int32_t y = bottom_tile; y <= top_tile && !hit_brick; ++y) {
        for (int32_t x = left_tile; x <= right_tile; ++x) {
            if (x < 0 || x >= int32_t(PPU466::BackgroundWidth) || y < 0 || y >= int32_t(PPU466::BackgroundHeight)) 
            {
                continue;
            }
            uint32_t index = x + y * PPU466::BackgroundWidth;
            if (ppu.background[index] != 0) 
            {
                ppu.background[index] = 0;
                remaining_bricks -= 1;
                ball_velocity.y = -ball_velocity.y;
                hit_brick = true;
                break;
            }
        }
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
    for (uint8_t i = 0; i < paddle_tiles; ++i) {

        ppu.sprites[i].x =
            uint8_t(paddle_at.x) + 8 * i;

        ppu.sprites[i].y =
            uint8_t(paddle_at.y);

        ppu.sprites[i].index = 1;
        ppu.sprites[i].attributes = 0;
    }

    //draw ball:
    ppu.sprites[6].x =
        uint8_t(ball_at.x);

    ppu.sprites[6].y =
        uint8_t(ball_at.y);

    ppu.sprites[6].index = 2;
    ppu.sprites[6].attributes = 1;

    ppu.draw(drawable_size);
}
