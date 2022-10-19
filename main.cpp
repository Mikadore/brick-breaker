#include <array>
#include <span>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

constexpr auto HEIGHT = 900;
constexpr auto WIDTH = 560;


struct Brick {
    bool                alive;
    sf::RectangleShape  shape;

    Brick() : shape(sf::Vector2f(WIDTH/14, WIDTH/30)), alive(true) {
        shape.setFillColor(sf::Color::Red);
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(1);
    }
};


void initBricks(std::span<Brick> bricks) {
    size_t x = 0;
    size_t y = 0;
    
    for (auto& b : bricks) {
        b.shape.setPosition(sf::Vector2f(x, y));
        x += b.shape.getSize().x;
        if (x >= WIDTH) {
            y += b.shape.getSize().y;
            x = 0;
        }
    }
}

struct Ball {
    sf::CircleShape     shape;
    sf::Vector2f        velocity;
    

    Ball() : shape(10), velocity(0, -200) {
        shape.setFillColor(sf::Color::White);
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(1);
        shape.setOrigin(sf::Vector2f(10,10));
        shape.setPosition(sf::Vector2f(WIDTH/2, 2*(HEIGHT/3)));

        auto randAngle = -45 + (rand() % 91);
        setAngle(randAngle*M_PI/180.0);
    }

    void move(float seconds) {
        shape.move(velocity * seconds);
    }

    void setAngle(float radians) {
        velocity.x = velocity.x * std::cos(radians) - velocity.y * std::sin(radians);
        velocity.y = velocity.x * std::sin(radians) + velocity.y * std::cos(radians);
    }

    void bounceIf() {
        if (shape.getPosition().x <= 0 || shape.getPosition().x >= WIDTH - shape.getRadius()) {
            velocity.x *= -1;
        } 
        if (shape.getPosition().y <= 0) {
            velocity.y *= -1;
        }
    }

    bool isDead() {
        return shape.getPosition().y >= HEIGHT - shape.getRadius();
    }

    bool collides(const sf::RectangleShape& rs) {
        auto center = sf::Vector2f(shape.getPosition().x + shape.getRadius(), shape.getPosition().y + shape.getRadius());
        auto aabb_half_extents = sf::Vector2f(rs.getSize().x/2.0f, rs.getSize().y / 2.0f);
        auto aab_center = sf::Vector2f(rs.getPosition().x + aabb_half_extents.x, rs.getPosition().y + aabb_half_extents.y);
        auto difference = center - aab_center;
        auto clamped = sf::Vector2f(
            std::clamp(difference.x, -aabb_half_extents.x, aabb_half_extents.x),
            std::clamp(difference.y, -aabb_half_extents.y, aabb_half_extents.y));
        auto closest = aab_center + clamped;
        difference = closest - center;

        printf("CCenter: %f %f\n", center.x, center.y);
        printf("AHalves: %f %f\n", aabb_half_extents.x, aabb_half_extents.y);
        printf("ACenter: %f %f\n", aab_center.x, aab_center.y);
        printf("Diffrnc: %f %f\n", difference.x, difference.y);
        printf("Clamped: %f %f\n", clamped.x, clamped.y);
        printf("Closest: %f %f\n", closest.x, closest.y);
        printf("Mgntude: %f\n", std::sqrt(difference.x*difference.x + difference.y*difference.y));
        return std::sqrt(difference.x*difference.x + difference.y*difference.y) < shape.getRadius();
    }
};

struct Paddle {
    sf::RectangleShape  shape;

    Paddle() : shape(sf::Vector2f(200,15)) {
        shape.setFillColor(sf::Color::White);
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(1);
        shape.setPosition(sf::Vector2f(WIDTH/2 - 100, HEIGHT - 45));
    }

    void update() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            if (shape.getPosition().x > 0) shape.move(sf::Vector2f(-0.5, 0));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            if (shape.getPosition().x < (WIDTH - 200)) shape.move(sf::Vector2f(0.5, 0));
        }
    }
};

int main() {
    std::srand(std::time(NULL));
    //sf::Font pixeboy;
    //pixeboy.loadFromFile("../Pixeboy-z8XDG.ttf");
    //sf::Text scoreCounter;
    //scoreCounter.setFont(pixeboy);
    //scoreCounter.setFillColor(sf::Color::Black);
    //scoreCounter.setCharacterSize(30);
    //scoreCounter.setPosition(sf::Vector2f(0, WIDTH-30));
    //scoreCounter.setString("Score: 0");

    sf::RenderWindow window{sf::VideoMode(WIDTH, HEIGHT), "Brick Breaker!", sf::Style::Close};

    std::array<Brick, 140> bricks{};
    initBricks(bricks);

    Ball ball;
    Paddle paddle;

    auto score = 0;    
    sf::Clock   clock;
    while (window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e)) {
            switch (e.type) {
                case sf::Event::Closed: {
                    window.close();
                    return EXIT_SUCCESS;
                }
            }
        }
        window.clear(sf::Color(52, 107, 235));

        for (const auto& b : bricks) {
            if (b.alive) {
                //if (ball.collides(b.shape)) {
                //    ball.shape.setFillColor(sf::Color::Red);
                //}
                window.draw(b.shape);
            }
        }

        ball.move(clock.restart().asSeconds());
        if (ball.isDead()) {
            printf("Game Over!\n");
            window.close();
        }
        ball.bounceIf();
        if (ball.collides(paddle.shape)) {
            ball.velocity.y *= -1;
        }
        window.draw(ball.shape);
        auto border = sf::RectangleShape(sf::Vector2f(2*ball.shape.getRadius(), 2*ball.shape.getRadius()));
        border.setPosition(ball.shape.getPosition() - sf::Vector2f(ball.shape.getRadius(), ball.shape.getRadius()));
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineColor(sf::Color::Black);
        border.setOutlineThickness(1);
        window.draw(border);

        ball.shape.setFillColor(sf::Color::White); 

        paddle.update();
        window.draw(paddle.shape);

        //window.draw(scoreCounter);

        window.display();
    }
    
}