#include "Neural_net.h"
#include <random>

float sigmoid_derivative(float x) {
	return x * (1 - x);
}

float sigmoid(float x) {
	if (x > 5) {
		return 1;
	}
	else if (x < -5) {
		return 0;
	}
	else {
		return 1 / (1 + exp(-x));
	}
}

Neural_net::Neural_net(int in_INPUT_NEURONS, int in_HIDDEN_NEURONS, int in_OUTPUT_NEURONS, int in_HIDDEN_LAYERS) {
	std::mt19937 generator(rand() % 30);
	std::uniform_real_distribution<float> value_distribution(-1.0, 1.0);

	LEARNING_RATE = 0.01;
	INPUT_NEURONS = in_INPUT_NEURONS;
	HIDDEN_NEURONS = in_HIDDEN_NEURONS;
	OUTPUT_NEURONS = in_OUTPUT_NEURONS;
	HIDDEN_LAYERS = in_HIDDEN_LAYERS;
	LAYERS = HIDDEN_LAYERS + 2;

	neural_network.resize(LAYERS);
	neural_network[0].resize(INPUT_NEURONS);
	for (int i = 0; i < HIDDEN_LAYERS; i++) {
		neural_network[i + 1].resize(HIDDEN_NEURONS);
	}
	neural_network[LAYERS - 1].resize(OUTPUT_NEURONS);

	//weights resize
	weights.resize((LAYERS - 1));
	for (int i = 0; i < weights.size(); i++) {

		weights[i].resize(neural_network[i + 1].size());

		for (int j = 0; j < weights[i].size(); j++) {

			weights[i][j].resize(neural_network[i].size());

		}
	}
	for (int i = 0; i < weights.size(); i++) {

		for (int j = 0; j < weights[i].size(); j++) {

			for (int k = 0; k < weights[i][j].size(); k++) {

				weights[i][j][k] = value_distribution(generator);

			}
		}
	}

	//biases resize
	biases.resize(HIDDEN_LAYERS + 1);
	for (int i = 0; i < HIDDEN_LAYERS; i++) {
		biases[i].resize(HIDDEN_NEURONS);
	}
	biases[biases.size() - 1].resize(OUTPUT_NEURONS);

	for (int i = 0; i < biases.size(); i++) {
		for (int j = 0; j < biases[i].size(); j++) {
			biases[i][j] = 0;
		}
	}

	//errors resize
	errors.resize(HIDDEN_LAYERS + 1);
	for (int i = 0; i < HIDDEN_LAYERS; i++) {
		errors[i].resize(HIDDEN_NEURONS);
	}
	errors[errors.size() - 1].resize(OUTPUT_NEURONS);

	//other resize
	inputs.resize(INPUT_NEURONS);
	expected_outputs.resize(OUTPUT_NEURONS);
	//END INIT
}

void Neural_net::set_inputs(vector_1d& ins) {
	inputs = ins;
}

void Neural_net::set_expoutputs(vector_1d& expout) {
	expected_outputs = expout;
}

vector_1d Neural_net::forward_propagation() {
	//inputs assignment
	for (int i = 0; i < inputs.size(); i++) {
		neural_network[0][i] = inputs[i];
	}

	//res neurons
	for (int i = 0; i < neural_network.size() - 1; i++) {
		for (int j = 0; j < neural_network[i + 1].size(); j++) {
			neural_network[i + 1][j] = 0;
		}
	}

	//forward propagation
	for (int i = 0; i < weights.size(); i++) {

		for (int j = 0; j < weights[i].size(); j++) {

			for (int k = 0; k < weights[i][j].size(); k++) {

				neural_network[1 + i][j] += neural_network[i][k] * weights[i][j][k];

			}

			neural_network[i + 1][j] += biases[i][j];


			neural_network[1 + i][j] = sigmoid(neural_network[1 + i][j]);

		}

	}



	//return outputs
	return neural_network[neural_network.size() - 1];

}

void Neural_net::back_propagation() {
	//res errors
	for (int i = 0; i < errors.size(); i++) {
		for (int j = 0; j < errors[i].size(); j++) {
			errors[i][j] = 0;
		}
	}


	//1st layer of weights (closest to output)
	for (int i = 0; i < weights[weights.size() - 1].size(); i++) {
		for (int j = 0; j < weights[weights.size() - 1][i].size(); j++) {
			weights[weights.size() - 1][i][j] -= LEARNING_RATE * 2 * (neural_network[neural_network.size() - 1][i] - expected_outputs[i]) * sigmoid_derivative(neural_network[neural_network.size() - 1][i]) * neural_network[neural_network.size() - 2][j];
		}
	}

	//main errors
	for (int i = 0; i < errors[errors.size() - 1].size(); i++) {
		errors[errors.size() - 1][i] = pow(neural_network[neural_network.size() - 1][i] - expected_outputs[i], 1);
	}

	//errors in other layers
	for (int i = 0; i < weights.size() - 1; i++) {

		for (int j = 0; j < weights[weights.size() - 1 - i].size(); j++) {

			for (int k = 0; k < errors[errors.size() - 2 - i].size(); k++) {

				errors[errors.size() - 2 - i][k] += errors[errors.size() - 1 - i][j] * weights[weights.size() - 1 - i][j][k] * sigmoid_derivative(neural_network[neural_network.size() - 1 - i][j]);

			}

		}

	}


	//other layers weights alteration
	for (int i = 0; i < weights.size() - 1; i++) {

		for (int j = 0; j < weights[weights.size() - 2 - i].size(); j++) {

			for (int k = 0; k < weights[weights.size() - 2 - i][j].size(); k++) {


				weights[weights.size() - 2 - i][j][k] -= LEARNING_RATE * 2 * errors[weights.size() - 2 - i][j] * sigmoid_derivative(neural_network[weights.size() - 1 - i][j]) * neural_network[weights.size() - 2 - i][k];


			}
		}
	}

	//biases alteration
	for (int i = 0; i < biases.size(); i++) {
		for (int j = 0; j < biases[i].size(); j++) {

			biases[i][j] -= LEARNING_RATE * 2 * errors[i][j];

		}
	}

}
