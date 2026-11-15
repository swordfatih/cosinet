module;

#include <stdexcept>
#include <vector>

export module cosinet.dimensional_perceptron;

import cosinet.vector;

export namespace cosinet
{

template <size_t dimension>
class dimensional_perceptron
{
public:
    struct sample
    {
        vector<float, dimension> input;
        int group_factor;
    };

    dimensional_perceptron()
    {
    }

    int evaluate(const vector<float, dimension>& input)
    {
        float sum = m_bias;

        for(size_t i = 0; i < dimension; ++i)
        {
            sum += input[i] * m_weights[i];
        }

        // heavyside activation function
        return sum >= 0.f ? 1 : -1;
    }

    void adjust(const sample& sample)
    {
        for(size_t i = 0; i < dimension; ++i)
        {
            m_weights[i] += sample.group_factor * sample.input[i] * m_learning_step;
        }

        m_bias += sample.group_factor * m_learning_step;
    }

    void train(const std::vector<sample>& samples, size_t epoch_count = 100000)
    {
        for(size_t epoch = 0; epoch < epoch_count; ++epoch)
        {
            bool mistakes = false;

            for(auto& sample: samples)
            {
                if(evaluate(sample.input) != sample.group_factor)
                {
                    adjust(sample);
                    mistakes = true;
                }
            }

            if(!mistakes)
            {
                break;
            }
        }
    }

    vector<float, dimension> boundary() const
    {
        if(m_weights.b == 0.f)
        {
            throw std::runtime_error("Boundary has no slope-intercept form");
        }

        // cartesian equation : ax + by + c
        // with a = weights.x, b = weights.y, c = bias
        //
        // slope intercept form :
        // slope = -a / b
        // intercept = -c / b
        return {-m_weights.a / m_weights.b, -m_bias / m_weights.b};
    }

private:
    vector<float, dimension> m_weights{};
    float m_learning_step = 1.f;
    float m_bias = 0.f;
};

} // namespace cosinet