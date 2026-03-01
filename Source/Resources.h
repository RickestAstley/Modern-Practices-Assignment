#pragma once
#include <vector>
#include <array>
#include "raylib.h"

class Texture {

public: Texture() : texture{ 0 } {}

      explicit Texture(const std::string& path) {
          texture = LoadTexture(path.c_str());
          if (texture.id == 0) {
              throw std::runtime_error("Failed to load texture: " + path);
          }
      }

	  Texture(const Texture&) = delete;
	  Texture& operator=(const Texture&) = delete;

	  Texture(Texture&& other) noexcept : texture(std::exchange(other.texture, { 0 })) {}

      Texture& operator=(Texture&& other) noexcept {
          if (this != &other) {
              UnloadTexture(texture);
              texture = std::exchange(other.texture, { 0 });
          }
          return *this;
      }

      ~Texture() {
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
    std::array<Texture, 3> shipTextures;
    Texture alienTexture{};
    Texture barrierTexture{};
    Texture laserTexture{};
};