/**
* @file SUI_in_texture_sdl_manager.h
* @brief manager the sdl_texture
*/
#ifndef SUI_IN_TEXTURE_SDL_MANAGER_H
#define SUI_IN_TEXTURE_SDL_MANAGER_H

#include <unordered_map>
#include "SDL_render.h"

namespace sui {

struct Texture_sdl_manager_unit {
    SDL_Texture *texture;
    SDL_Renderer *prenderer;
    bool static_access;
    bool valid;
    long long id;
};

class Texture_sdl_manager {
public:
    static Texture_sdl_manager *instance();
    SDL_Texture *get_texture(long long id);
    SDL_Renderer *get_renderer(long long id);
    // if the prenderer is nullptr, this function actually will destroy the texture only
    SDL_Texture *set_texture(long long id, SDL_Renderer *prenderer, Uint32 format, int access, int w, int h, SDL_BlendMode blend_mode = SDL_BlendMode::SDL_BLENDMODE_BLEND);
    SDL_Texture *set_texture(long long id, SDL_Renderer *prenderer, SDL_Surface *surface, SDL_BlendMode blend_mode = SDL_BlendMode::SDL_BLENDMODE_BLEND);
    // SDL_Texture *set_texture(long long id, SDL_Renderer *prenderer, SDL_Texture *pTexture);
    bool destroy_texture(long long id);
    bool set_texture_blend_mode(long long id, SDL_BlendMode blend_mode);
    // bool set_texture_update_func(long long id, std::function<SDL_Texture *()> func, bool auto_update_flag = true);
    // bool set_auto_update(long long id, bool auto_update_flag);
    bool is_valid(long long id);
    void invalid_texture(SDL_Renderer *prenderer);
    void invalid_texture(long long id);
    long long alloc_texture_id();
    bool free_texture_id(long long id);
    ~Texture_sdl_manager();
private:
    Texture_sdl_manager();
    static long long curr_id;
    std::unordered_map<long long, Texture_sdl_manager_unit> texture_map;
};

}
#endif