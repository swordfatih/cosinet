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
import cosinet.vector;
import cosinet.dimensional_perceptron;

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

    /* we check if the point is higher or lower the given line */
    bool classifier(const cosinet::vector<float, 2>& point, float slope, float intercept)
    {
        float projection = slope * point.a + intercept;
        return point.b > projection;
    }

    auto generate_samples(float slope, float intercept)
    {
        std::mt19937 random_generator{std::random_device{}()};
        std::uniform_real_distribution<float> random_distribution{-1.f, 1.f};

        std::vector<cosinet::dimensional_perceptron<2>::sample> samples;

        for(size_t i = 0; i < 1000; ++i)
        {
            auto input = cosinet::vector<float, 2>{
                random_distribution(random_generator),
                random_distribution(random_generator),
            };

            samples.push_back({
                .input = input,
                .group_factor = classifier(input, slope, intercept) ? 1 : -1,
            });
        }

        return samples;
    }

    auto generate_circles(
        const std::vector<cosinet::dimensional_perceptron<2>::sample>& samples,
        float slope,
        float intercept
    )
    {
        std::vector<sf::CircleShape> circles;
        for(const auto& sample: samples)
        {
            sf::CircleShape circle(2);

            // scale coordinates to make it visible on window
            circle.setPosition({(sample.input.a + 1.f) * 500.f, (sample.input.b + 1.f) * 500.f});

            circle.setFillColor(sample.group_factor < 0 ? sf::Color::Red : sf::Color::Blue);
            circles.push_back(circle);
        }

        return circles;
    }

    auto generate_boundary(const std::vector<cosinet::dimensional_perceptron<2>::sample>& samples, float slope, float intercept)
    {
        cosinet::dimensional_perceptron<2> perceptron;
        perceptron.train(samples);

        return perceptron.boundary();
    }

    auto generate_line(const cosinet::vector<float, 2>& boundary)
    {
        const float width = static_cast<float>(m_window.getSize().x);
        auto scaled_intercept = (boundary.b + 1.f - boundary.a) * 500.f;
        std::vector<sf::Vertex> line = {
            {{0.f, scaled_intercept}, sf::Color::Green},
            {{width, boundary.a * width + scaled_intercept}, sf::Color::Green},
        };
        return line;
    }

    void update()
    {
        float slope = 0.1f;
        float intercept = 0.5f;

        auto samples = generate_samples(slope, intercept);
        auto circles = generate_circles(samples, slope, intercept);
        auto boundary = generate_boundary(samples, slope, intercept);
        auto line = generate_line(boundary);

        auto values = centui::text<caption>{
            std::format("User: {} {}. Perceptron: {} {}", slope, intercept, boundary.a, boundary.b)
        };

        const auto update_slope = [&](float value) mutable
        {
            slope = value;
            values->set_text(std::format("User: {} {}. Perceptron: {} {}", slope, intercept, boundary.a, boundary.b));
            samples = generate_samples(slope, intercept);
            circles = this->generate_circles(samples, slope, intercept);
        };

        const auto update_intercept = [&](float value) mutable
        {
            intercept = value;
            values->set_text(std::format("User: {} {}. Perceptron: {} {}", slope, intercept, boundary.a, boundary.b));
            samples = generate_samples(slope, intercept);
            circles = this->generate_circles(samples, slope, intercept);
        };

        const auto train_and_evaluate = [&]() mutable
        {
            boundary = this->generate_boundary(samples, slope, intercept);
            line = this->generate_line(boundary);
            values->set_text(std::format("User: {} {}. Perceptron: {} {}", slope, intercept, boundary.a, boundary.b));
        };

        auto root = centui::container<overlay>{
            centui::container<panel>{
                centui::text<caption>{"Edit line"},
                values,
                centui::range<control>{{
                    .minimum = -1,
                    .maximum = 1,
                    .step = 0.01,
                    .value = slope,
                    .on_change = update_slope,
                }},
                centui::range<control>{{
                    .minimum = -1,
                    .maximum = 1,
                    .step = 0.01,
                    .value = intercept,
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
