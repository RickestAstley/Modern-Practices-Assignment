#pragma once
#include <vector>
#include <array>
#include "raylib.h"
#include <string>
#include <stdexcept>
#include <utility>

class TextureWrapper {

public: TextureWrapper() : texture{ 0 } {}

      explicit TextureWrapper(const std::string& path) {
          texture = LoadTexture(path.c_str());
          if (texture.id == 0) {
              throw std::runtime_error("Failed to load texture: " + path);
          }
      }

	  TextureWrapper(const TextureWrapper&) = delete;
	  TextureWrapper& operator=(const TextureWrapper&) = delete;

	  TextureWrapper(TextureWrapper&& other) noexcept : texture(std::exchange(other.texture, { 0 })) {}

      TextureWrapper& operator=(TextureWrapper&& other) noexcept {
          if (this != &other) {
              UnloadTexture(texture);
              texture = std::exchange(other.texture, { 0 });
          }
          return *this;
      }

      ~TextureWrapper() {
          if (texture.id != 0) {
              UnloadTexture(texture);
          }
      }

     operator Texture2D() const { return texture; }

private:
    Texture2D texture;
};


class Resources 
{
    public:

		Resources();

    // Using std::array for fixed-size collections
    std::array<TextureWrapper, 3> shipTextures;
    TextureWrapper alienTexture{};
    TextureWrapper barrierTexture{};
    TextureWrapper laserTexture{};
};