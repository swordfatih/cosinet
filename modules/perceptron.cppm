module;

#include <vector>
#include <stdexcept>

export module cosinet.perceptron;

export namespace cosinet
{

struct point
{
    float x;
    float y;
    int group = 0;
};

class perceptron
{
public:
    perceptron()
    {
    }

    int evaluate(point input)
    {
        float sum = input.x * m_weights.x + input.y * m_weights.y + m_bias;
        return sum >= 0.f ? 1 : -1;
    }

    void adjust(point input)
    {
        m_weights.x += input.group * input.x * m_learning;
        m_weights.y += input.group * input.y * m_learning;
        m_bias += input.group * m_learning;
    }

    void train(std::vector<point>& points)
    {
        for(int epoch = 0; epoch < 100000; ++epoch)
        {
            bool mistakes = false;

            for(auto& point: points)
            {
                if(evaluate(point) != point.group)
                {
                    adjust(point);
                    mistakes = true;
                }
            }

            if(!mistakes)
            {
                break;
            }
        }
    }

    point boundary() const
    {
        if(m_weights.y == 0.f)
        {
            throw std::runtime_error("Boundary has no slope-intercept form");
        }

        return {
            .x = -m_weights.x / m_weights.y,
            .y = -m_bias / m_weights.y,
        };
    }

private:
    point m_weights{0.f, 0.f};
    float m_learning = 1.f;
    float m_bias = 0.f;
};

} // namespace cosinet