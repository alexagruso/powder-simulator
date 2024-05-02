#include "Application.hpp"

#include "application/events/ApplicationExitEvent.hpp"
#include "config/Config.hpp"
#include "physics/Element.hpp"
#include "ui/ElementButton.hpp"
#include "ui/board/BoardDisplay.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

#include <vector>

using namespace Powder;

Application::Application()
{
    this->window = new sf::RenderWindow{
        sf::VideoMode{Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT},
        Config::WINDOW_TITLE, sf::Style::Default
    };

    this->window->setFramerateLimit(Config::FRAMERATE_LIMIT);

    this->events = std::stack<Event*>{};

    EntityPositioning positioning;
    positioning.horizontal = HorizontalPositioning::LEFT;
    positioning.vertical = VerticalPositioning::TOP;

    //  TODO: this is temporary, make separate UI handler
    ElementButton* fireButton = new ElementButton{
        {50, 50},
        {300, 0},
        positioning, new Physics::Fire{}
    };

    ElementButton* plantButton = new ElementButton{
        {50, 50},
        {300, 50},
        positioning, new Physics::Plant{}
    };

    ElementButton* woodButton = new ElementButton{
        {50, 50},
        {300, 100},
        positioning, new Physics::Wood{}
    };

    ElementButton* stoneButton = new ElementButton{
        {50, 50},
        {300, 150},
        positioning, new Physics::Stone{}
    };

    ElementButton* waterButton = new ElementButton{
        {50, 50},
        {300, 200},
        positioning, new Physics::Water{}
    };

    UI::BoardDisplay* board = new UI::BoardDisplay{
        {100, 100},
        new Physics::Fire{},
    };

    this->entities.push_back(fireButton);
    this->entities.push_back(plantButton);
    this->entities.push_back(woodButton);
    this->entities.push_back(stoneButton);
    this->entities.push_back(waterButton);
    this->entities.push_back(board);
}

Application::~Application()
{
    delete this->window;

    for (UIEntity* entity : this->entities)
    {
        delete entity;
    }
}

void Application::start()
{
    while (this->window->isOpen())
    {
        this->tick();
    }
}
void Application::tick()
{
    sf::Event systemEvent;

    while (this->window->pollEvent(systemEvent))
    {
        switch (systemEvent.type)
        {
            case sf::Event::Closed:
            {
                this->events.push(new ApplicationExitEvent{});
                break;
            }
            case sf::Event::KeyPressed:
            {
                this->inputManager.processKeyPress(systemEvent.key.code);
                break;
            }
            case sf::Event::KeyReleased:
            {
                this->inputManager.processKeyRelease(systemEvent.key.code);
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                this->inputManager.processMousePress(systemEvent.mouseButton.button,
                                                     {systemEvent.mouseButton.x, systemEvent.mouseButton.y});
                break;
            }
            case sf::Event::MouseButtonReleased:
            {
                this->inputManager.processMouseRelease(systemEvent.mouseButton.button,
                                                       {systemEvent.mouseButton.x, systemEvent.mouseButton.y});
                break;
            }
            case sf::Event::MouseMoved:
            {
                this->inputManager.processMouseMovement({systemEvent.mouseMove.x, systemEvent.mouseMove.y});
                break;
            }
            case sf::Event::MouseWheelScrolled:
            {
                //  TODO: add mouse wheel scroll to input manager
                break;
            }
            default: break;
        }
    }

    // Clones the input status into a new input event
    this->events.push(this->inputManager.inputStatus->clone());

    while (!this->events.empty())
    {
        Event* currentEvent = this->events.top();
        this->events.pop();

        // Global application events are handled first
        if (Event::isOfType<ApplicationExitEvent>(currentEvent))
        {
            this->window->close();
            this->exiting = true;
        }
        else if (auto check = Event::isOfType<InputEvent>(currentEvent))
        {
            auto event = check.value();

            if (event->queryKey(sf::Keyboard::Escape, InputStatus::PRESSED))
            {
                this->events.push(new ApplicationExitEvent{});
            }
        }

        // Handle events for each UI entity
        for (UIEntity* entity : this->entities)
        {
            auto newEvents = entity->handleEvent(currentEvent);

            for (Event* event : newEvents)
            {
                this->events.push(event);
            }
        }

        delete currentEvent;
    }

    // compute logic for each entity
    for (UIEntity* entity : entities)
    {
        std::vector<Event*> newEvents = entity->tick();

        for (Event* event : newEvents)
        {
            this->events.push(event);
        }
    }

    this->window->clear(Config::WINDOW_CLEAR_COLOR);

    // render all entities
    for (UIEntity* entity : entities)
    {
        entity->render(this->window);
    }

    this->window->display();

    // reset input status but maintain HELD and IDLE statuses across frames
    this->inputManager.reset();
}
