module;

#include <memory>
#include <random>
#include <vector>

#include <centui/backends/sfml.hpp>
#include <centui/centui.hpp>
#include <centui/platforms/sfml.hpp>

#include <SFML/Graphics.hpp>

export module cosinet.app;

import cosinet.style;
import cosinet.perceptron;

export namespace cosinet
{

class app
{
public:
    app()
        : m_window({sf::VideoMode{{1200, 1200}}, "CentUI + SFML = <3"}),
          m_platform(m_window),
          m_backend(m_platform),
          m_ui(m_platform, m_backend)
    {
        m_window.setFramerateLimit(60);
    }

    void process_events(centui::document& document)
    {
        while(const auto event = m_window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            {
                m_window.close();
                break;
            }

            m_ui.process_event(document, *event);

            if(event->is<sf::Event::Resized>())
            {
                const auto size = m_window.getSize();

                if(size.x && size.y)
                {
                    m_window.setView(
                        sf::View{sf::FloatRect{{0.f, 0.f}, {static_cast<float>(size.x), static_cast<float>(size.y)}}}
                    );
                }
            }
        }
    }

    std::vector<cosinet::point> generate_points()
    {
        std::mt19937 random_generator{std::random_device{}()};
        std::uniform_real_distribution<float> random_distribution{0.f, 1000.f};

        std::vector<cosinet::point> points;

        for(size_t i = 0; i < 1000; ++i)
        {
            points.push_back(
                cosinet::point{
                    .x = random_distribution(random_generator),
                    .y = random_distribution(random_generator),
                }
            );
        }

        return points;
    }

    std::vector<sf::CircleShape> generate_circles(std::vector<cosinet::point>& points, float slope, float intercept)
    {
        std::vector<sf::CircleShape> circles;
        for(auto& point: points)
        {
            sf::CircleShape circle(2);
            circle.setPosition({point.x, point.y});

            float ordinate = slope * point.x + intercept;
            point.group = (point.y > ordinate) ? 1 : -1;
            circle.setFillColor(point.group > 0 ? sf::Color::Red : sf::Color::Blue);

            circles.push_back(circle);
        }

        return circles;
    }

    auto generate_boundary(std::vector<cosinet::point> points)
    {
        cosinet::perceptron perceptron;
        perceptron.train(points);

        return perceptron.boundary();
    }

    auto generate_line(point& boundary)
    {
        const float width = static_cast<float>(m_window.getSize().x);
        std::vector<sf::Vertex> line = {
            {{0.f, boundary.y}, sf::Color::Green},
            {{width, boundary.x * width + boundary.y}, sf::Color::Green},
        };
        return line;
    }

    void update()
    {
        float slope = 0.1f;
        float intercept = 500.f;

        auto points = generate_points();
        auto circles = generate_circles(points, slope, intercept);
        
        auto boundary = generate_boundary(points);
        auto line = generate_line(boundary);
        
        auto values = centui::text<caption>{std::format("User: {} {}. Perceptron: {} {}", slope, intercept, boundary.x, boundary.y)};

        const auto update_slope = [&](float value) mutable
        {
            slope = value;
            values->set_text(std::format("User: {} {}. Perceptron: {} {}", slope, intercept, boundary.x, boundary.y));
            circles = this->generate_circles(points, slope, intercept);
        };

        const auto update_intercept = [&](float value) mutable
        {
            intercept = value;
            values->set_text(std::format("User: {} {}. Perceptron: {} {}", slope, intercept, boundary.x, boundary.y));
            circles = this->generate_circles(points, slope, intercept);
        };

        const auto train_and_evaluate = [&]() mutable
        {
            boundary = this->generate_boundary(points);
            line = this->generate_line(boundary);
            values->set_text(std::format("User: {} {}. Perceptron: {} {}", slope, intercept, boundary.x, boundary.y));
        };

        auto root = centui::container<overlay>{
            centui::container<panel>{
                centui::text<caption>{"Edit line"},
                values,
                centui::range<control>{{
                    .minimum = -10,
                    .maximum = 10,
                    .step = 0.01,
                    .value = 0.1,
                    .on_change = update_slope,
                }},
                centui::range<control>{{
                    .minimum = 0,
                    .maximum = 1000,
                    .step = 0.01,
                    .value = 500,
                    .on_change = update_intercept,
                }},
                centui::button<control>{
                    "Train and evaluate",
                    {
                        .on_click = train_and_evaluate,
                    }
                }
            },
        };

        centui::document document{root};
        document.set_style({.font = centypo::font{"Inter"}});

        while(m_window.isOpen())
        {
            process_events(document);

            m_window.clear(sf::Color::Black);

            for(auto& circle: circles)
            {
                m_window.draw(circle);
            }

            m_window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);

            m_ui.render(document);
            m_window.display();
        }
    }

    static std::string to_string()
    {
        return "Hello from starter!";
    }

private:
    sf::RenderWindow m_window;
    centui::sfml::platform m_platform;
    centui::sfml::backend m_backend;
    centui::ui<centui::sfml::platform> m_ui;
    centui::handle<> m_root;
};

} // namespace cosinet