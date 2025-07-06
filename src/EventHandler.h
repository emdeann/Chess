#pragma once

#include "constants/Enums.h"
#include "GameManager.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Rect.hpp>

class MousePressedEventHandler {
private:
    GameManager& manager;

    bool leftMousePressed(const sf::Event::MouseButtonPressed& event) {
        return event.button == sf::Mouse::Button::Left;
    }
public:
    MousePressedEventHandler(GameManager& manager) : manager(manager) {}


	void operator()(const sf::Event::MouseButtonPressed& event) {
        if (!leftMousePressed(event)) {
            return;
        }

        switch (manager.getUI().getWindowState()) {
        case WindowState::START:
            manager.tryStartGame(event);
            break;
        case WindowState::GAME:
            manager.runTurn(event);
            break;
        case WindowState::END:
            manager.tryRestartGame(event);
            break;
        }
	}
};

class WindowClosedEventHandler {
private:
    GameManager& manager;
public:
    WindowClosedEventHandler(GameManager& mgr) : manager(mgr) {}

    void operator()(const sf::Event::Closed& event) {
        manager.getUI().closeWindow();
    }
};