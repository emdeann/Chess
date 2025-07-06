#include "GameManager.h"
#include "EventHandler.h"
int main() {
    GameManager manager;

    MousePressedEventHandler onMousePressed(manager);
    WindowClosedEventHandler onClose(manager);

    while (manager.getUI().windowIsOpen()) {
        manager.handleEvents([&](const sf::Event::Closed& e) {onClose(e); }, [&](const sf::Event::MouseButtonPressed& e) {onMousePressed(e); });
        manager.updateScreen();
    }

}

