#ifndef POWDER_CONFIG
#define POWDER_CONFIG

#include "physics/Element.hpp"

#include <SFML/Graphics/Color.hpp>

#include <string>

namespace Powder::Config
{

const uint WINDOW_WIDTH = 800;
const uint WINDOW_HEIGHT = 600;

const uint FRAMERATE_LIMIT = 30;

const std::string WINDOW_TITLE = "Powder Simulator";

const sf::Color WINDOW_CLEAR_COLOR = sf::Color::Black;
const sf::Color VOID_COLOR = sf::Color::Black;
const sf::Color PUKE_PLACEHOLDER = sf::Color(0x89A203FF);

const uint BOARD_WIDTH = 100;
const uint BOARD_HEIGHT = 100;
const uint PARTICLE_PIXEL_SIZE = 2;

} // namespace Powder::Config

#endif // POWDER_CONFIG
