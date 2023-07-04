#pragma once
#include <vector>

typedef std::vector<float> vector_1d;
typedef std::vector<std::vector<float>> vector_2d;
typedef std::vector<std::vector<std::vector<float>>> vector_3d;

class Neural_net
{
private:
	vector_2d neural_network;
	vector_3d weights;
	vector_2d biases;
	vector_2d errors;
	vector_1d expected_outputs;
	vector_1d inputs;
	int INPUT_NEURONS;
	int HIDDEN_NEURONS;
	int OUTPUT_NEURONS;
	int HIDDEN_LAYERS;
	int LAYERS;
	float LEARNING_RATE;
public:
	Neural_net(int in_INPUT_NEURONS, int in_HIDDEN_NEURONS, int in_OUTPUT_NEURONS, int in_HIDDEN_LAYERS);
	void set_inputs(vector_1d& ins);
	void set_expoutputs(vector_1d& expout);
	vector_1d forward_propagation();
	void back_propagation();
};

