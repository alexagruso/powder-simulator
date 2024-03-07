#include "event/Event.hpp"
#include "ui/Button.hpp"
#include "util/Overloaded.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/WindowBase.hpp>

#include <cmath>
#include <iostream>
#include <optional>
#include <random>
#include <stack>
#include <variant>
#include <vector>

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

const unsigned int FRAMERATE_LIMIT = 60;

const std::string WINDOW_TITLE = "Powder Simulator";

const sf::Color WINDOW_FILL_COLOR = sf::Color::Black;

double tick = 0;
std::vector<Powder::Button> buttons;

const unsigned int BUTTON_COUNT = 25;

int main()
{
    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        buttons.push_back(Powder::Button{
            {0, 0},
            {0, 0}
        });
    }

    sf::RenderWindow window{
        sf::VideoMode{WINDOW_WIDTH, WINDOW_HEIGHT},
        WINDOW_TITLE, sf::Style::Default
    };

    window.setFramerateLimit(FRAMERATE_LIMIT);

    std::stack<Powder::Event> events;

    Powder::KeyboardEvent keyboardEvent;
    Powder::MouseEvent mouseEvent;

    while (window.isOpen())
    {
        using Powder::InputStatus;

        sf::Event systemEvent;

        while (window.pollEvent(systemEvent))
        {
            using Powder::InputStatus;

            //! Each branch of this switch statement is a different SFML event that can occur,
            //? I opted to not put this functionality into a member function because I don't intend for the
            //? std::bitset instances to be accessed directly from external objects.
            switch (systemEvent.type)
            {
                case sf::Event::Closed:
                {
                    events.push(Powder::ApplicationExitEvent{});
                }
                case sf::Event::KeyPressed:
                {
                    keyboardEvent.keys.set(systemEvent.key.code * 3 + static_cast<int>(InputStatus::PRESSED));
                    keyboardEvent.keys.set(systemEvent.key.code * 3 + static_cast<int>(InputStatus::ACTIVE));
                    break;
                }
                case sf::Event::KeyReleased:
                {
                    keyboardEvent.keys.set(systemEvent.key.code * 3 + static_cast<int>(InputStatus::RELEASED));
                    keyboardEvent.keys.reset(systemEvent.key.code * 3 + static_cast<int>(InputStatus::ACTIVE));
                    break;
                }
                case sf::Event::MouseButtonPressed:
                {
                    mouseEvent.buttons.set(systemEvent.key.code * 3 + static_cast<int>(InputStatus::PRESSED));
                    mouseEvent.buttons.set(systemEvent.key.code * 3 + static_cast<int>(InputStatus::ACTIVE));
                    break;
                }
                case sf::Event::MouseButtonReleased:
                {
                    mouseEvent.buttons.set(systemEvent.key.code * 3 + static_cast<int>(InputStatus::RELEASED));
                    mouseEvent.buttons.reset(systemEvent.key.code * 3 + static_cast<int>(InputStatus::ACTIVE));
                    break;
                }
                case sf::Event::MouseMoved:
                {
                    mouseEvent.moving = true;
                    mouseEvent.position = {systemEvent.mouseMove.x, systemEvent.mouseMove.y};
                    break;
                }
                default: break;
            }
        }

        events.push(keyboardEvent);
        events.push(mouseEvent);

        while (!events.empty())
        {
            using namespace Powder::Util;

            const Powder::Event& currentEvent = events.top();

            for (Powder::Button& button : buttons)
            {
                // TODO: refactor this into a general event handler interface
                button.handleEvent(currentEvent);
            }

            //!     This is how we pattern match with std::variant instances, please read and understand:
            //! Using the Overloaded<Ts...> class defined in util, we can pass a lambda for each
            //! type that we wish to check for. If we want to use an externally scoped variable inside
            //! the lambda, you include it in the lambda's capture, or in the []. We then pass the
            //! std::variant instance, in this case "currentEvent" to the std::visit function, and the
            //! rest is taken care of for us. Overloaded<Ts...> also requires a fallback case, and in this
            //! case it is implemented as an empty lambda with one argument of type auto.
            //?     See the Event source code files for more information on implementing std::variant types.
            std::visit(Overloaded{[&window](const Powder::ApplicationExitEvent& _) { window.close(); },
                                  [&window](const Powder::KeyboardEvent& event)
                                  {
                                      if (event.query(sf::Keyboard::Escape, InputStatus::PRESSED))
                                      {
                                          window.close();
                                      }
                                  },
                                  [](auto _) {}},
                       currentEvent);

            events.pop();
        }

        // Done with events

        // Reset input states

        for (int i = 0; i < sf::Keyboard::KeyCount; i++)
        {
            // <keyCode * 3 + status>
            keyboardEvent.keys.reset(i * 3 + static_cast<int>(InputStatus::PRESSED));
            keyboardEvent.keys.reset(i * 3 + static_cast<int>(InputStatus::RELEASED));
        }

        for (int i = 0; i < sf::Mouse::ButtonCount; i++)
        {
            // <keyCode * 3 + status>
            mouseEvent.buttons.reset(i * 3 + static_cast<int>(InputStatus::PRESSED));
            mouseEvent.buttons.reset(i * 3 + static_cast<int>(InputStatus::RELEASED));
        }

        // We want to recheck if the mouse is moving each frame
        mouseEvent.moving = false;

        // Drawing to window

        window.clear(WINDOW_FILL_COLOR);

        // logic for button circle
        for (int i = 0; i < buttons.size(); i++)
        {
            float xPosition =
                200 * cos(((tick + ((float) i / BUTTON_COUNT) * 360) * (M_PI / 180.0)) + WINDOW_WIDTH / 2.0) +
                WINDOW_HEIGHT / 2.0;
            float yPosition =
                200 * sin(((tick + ((float) i / BUTTON_COUNT) * 360) * (M_PI / 180.0)) + WINDOW_WIDTH / 2.0) +
                WINDOW_HEIGHT / 2.0;

            buttons.at(i).size = {30, 30};
            buttons.at(i).position = {xPosition, yPosition};

            buttons.at(i).tick(window);
        }

        window.display();

        tick += 0.5;
    }

    return 0;
}