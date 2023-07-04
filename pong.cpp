#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
#include <random>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <array>
#include "Neural_net.h"

class Ball {
private:
	sf::RectangleShape pilka;
	float v_up;
	float v_right;
	bool start;
	sf::Clock zegar_ruch;
	sf::Clock kolizja_up;
	sf::Clock kolizja_right;
public:
	Ball();
	void ruch();
	void rysuj(sf::RenderWindow& okno);
	void kolizja(float pos_prawa, float pos_lewa,bool&odb,bool&odb2);
	void reset();
	float getPosition_x();
	float getPosition_y();
};

Ball::Ball() {
	start = true;
	v_up = 4.5;
	v_right = 4;
	pilka.setSize(sf::Vector2f(10, 10));
	pilka.setPosition(sf::Vector2f(295, 150));

}

float Ball::getPosition_x() {
	return pilka.getPosition().x;
}

float Ball::getPosition_y() {
	return pilka.getPosition().y;
}

void Ball::reset() {
	pilka.setPosition(sf::Vector2f(295, 150));
	v_up = 2.5 + rand() % 4;
	int x = rand() % 2;
	if (x == 0) {
		v_right = 3;
	}
	else {
		v_right = -3;
	}
	start = true;
}

void Ball::ruch() {
	if (zegar_ruch.getElapsedTime().asSeconds() > 0.016) {
		pilka.setPosition(sf::Vector2f(pilka.getPosition().x + v_right, pilka.getPosition().y + v_up));
		zegar_ruch.restart();
	}
}

void Ball::rysuj(sf::RenderWindow& okno) {
	okno.draw(pilka);
}

void Ball::kolizja(float pos_prawa, float pos_lewa,bool &odb,bool&odb2) {
	if (kolizja_up.getElapsedTime().asSeconds() > 0.2) {
		if (pilka.getPosition().y < 150 || pilka.getPosition().y>590) {
			v_up = -v_up;
			kolizja_up.restart();
		}
	}
	if (kolizja_right.getElapsedTime().asSeconds() > 0.5) {
		
		if ((pilka.getPosition().y > pos_prawa && pilka.getPosition().y < pos_prawa + 80) && (pilka.getPosition().x > 550 && pilka.getPosition().x < 560)) {
			v_right = -v_right;
			v_up = 4 * sin((3.14 / 120) * (pilka.getPosition().y - pos_prawa - 40));
			kolizja_right.restart();
			odb2 = true;
			start = false;
		}
		else if ((pilka.getPosition().y > pos_lewa && pilka.getPosition().y < pos_lewa + 80) && (pilka.getPosition().x < 40 && pilka.getPosition().x>30)) {
			v_right = -v_right;
			v_up = 4 * sin((3.14 / 120) * (pilka.getPosition().y - pos_lewa - 40));
			kolizja_right.restart();
			start = false;
			odb = true;
		}
		
	}
	if (start == false) {
		if (v_right > 0) {
			v_right = 7;
		}
		else {
			v_right = -7;
		}
	}

}

class Game {
private:
	sf::RectangleShape paletkaLewa;
	sf::RectangleShape paletkaPrawa;
	Ball pilka;
	sf::RectangleShape gora;
	sf::Text punktyLewa;
	sf::Text punktyPrawa;
	sf::Clock ruch_palet_praw;
	sf::Clock ruch_palet_lew;
	sf::Clock getdan;
	sf::Clock getdane2;
	sf::Font czcionka;
	vector_2d dane;
	vector_2d outs;
	vector_2d expected;

	vector_2d daneright;
	vector_2d outsright;
	vector_2d expectedright;
	int punkty_prawa;
	int punkty_lewa;
	int iterator;
	int iterator2;
	bool punkt_zdobyty_AI;
	bool punkt_zdobyty_enemy;
	bool odbicie;
	bool odbicie2;
	Neural_net* neuralnet;
	Neural_net* neuralnetright;
public:
	Game(Neural_net*neunet, Neural_net* neunet2);
	void rysuj(sf::RenderWindow& okno);
	void gra(sf::Event&event, sf::RenderWindow& okno);
	void ruch_paletek(sf::Event&event);
	void punkty_check();
	void AI_enemy();
	void AI_move();
	void AI_getdane();
	void AI_learn();
	void AI_player();
	void reset();
};

Game::Game(Neural_net* neunet, Neural_net* neunet2) {
	neuralnet = neunet;
	neuralnetright = neunet2;

	//150 do 600
	gora.setSize(sf::Vector2f(600, 5));
	gora.setPosition(sf::Vector2f(0, 145));

	paletkaLewa.setSize((sf::Vector2f(10, 80)));
	paletkaLewa.setPosition(sf::Vector2f(30, 350));

	paletkaPrawa.setSize((sf::Vector2f(10, 80)));
	paletkaPrawa.setPosition(sf::Vector2f(560, 350));

	
	czcionka.loadFromFile("Arialn.ttf");
	
	punktyLewa.setFont(czcionka);
	punktyPrawa.setFont(czcionka);

	punktyLewa.setCharacterSize(40);
	punktyLewa.setPosition(sf::Vector2f(100, 55));
	punktyLewa.setString(std::to_string(punkty_lewa));

	punktyPrawa.setCharacterSize(40);
	punktyPrawa.setPosition(sf::Vector2f(450, 55));
	punktyPrawa.setString(std::to_string(punkty_prawa));

	punkty_lewa = 0;
	punkty_prawa = 0;
	dane.resize(20);
	outs.resize(20);
	expected.resize(20);
	for (int i = 0; i < 20; i++) {
		dane[i].resize(2);
		outs[i].resize(1);
		expected[i].resize(1);
	}
	daneright.resize(20);
	outsright.resize(20);
	expectedright.resize(20);
	for (int i = 0; i < 20; i++) {
		daneright[i].resize(2);
		outsright[i].resize(1);
		expectedright[i].resize(1);
	}

	
	iterator = 0;
	iterator2 = 0;
	punkt_zdobyty_AI = false;
	punkt_zdobyty_enemy = false;
	odbicie = false;
	odbicie2 = false;
}

void Game::ruch_paletek(sf::Event& event) {
	if(ruch_palet_praw.getElapsedTime().asSeconds()>0.009){
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Down) {
				if (paletkaPrawa.getPosition().y < 520) {
					paletkaPrawa.setPosition(sf::Vector2f(paletkaPrawa.getPosition().x, paletkaPrawa.getPosition().y + 3));
					ruch_palet_praw.restart();
				}
			}
			else if (event.key.code == sf::Keyboard::Up) {
				if (paletkaPrawa.getPosition().y > 150) {
					paletkaPrawa.setPosition(sf::Vector2f(paletkaPrawa.getPosition().x, paletkaPrawa.getPosition().y - 3));
					ruch_palet_praw.restart();
				}
			}
		}
	}
}

void Game::punkty_check() {
	if (pilka.getPosition_x() > 600) {
		punkty_lewa++;
		punktyLewa.setString(std::to_string(punkty_lewa));
		reset();
		punkt_zdobyty_AI = true;
	}
	else if (pilka.getPosition_x() < 0) {
		punkty_prawa++;
		punktyPrawa.setString(std::to_string(punkty_prawa));
		reset();
		punkt_zdobyty_enemy = true;
	}
}

void Game::rysuj(sf::RenderWindow& okno) {
	okno.draw(paletkaLewa);
	okno.draw(paletkaPrawa);
	okno.draw(gora);
	okno.draw(this->punktyLewa);
	okno.draw(punktyPrawa);
	pilka.rysuj(okno);
}

void Game::gra(sf::Event& event,sf::RenderWindow&okno) {
	ruch_paletek(event);
	AI_enemy();
	AI_player();
	pilka.ruch();
	pilka.kolizja(paletkaPrawa.getPosition().y, paletkaLewa.getPosition().y, odbicie, odbicie2);
	punkty_check();
	rysuj(okno);
}

void Game::AI_enemy() {
	AI_move();
	AI_getdane();
	AI_learn();
}

void Game::AI_move() {

	if (ruch_palet_lew.getElapsedTime().asSeconds() > 0.02) {
		float x_diff_ball = (pilka.getPosition_x() - 30) / 10;
		float y_diff_ball = (pilka.getPosition_y() - paletkaLewa.getPosition().y-40)/5;
		
		vector_1d ins;
		ins.resize(2);
		ins[0] = x_diff_ball;
		ins[1] = y_diff_ball;
		
		neuralnet->set_inputs(ins);

		vector_1d out = neuralnet->forward_propagation();

		if (out[0] > 0.5 && paletkaLewa.getPosition().y > 150) {
			paletkaLewa.setPosition(sf::Vector2f(paletkaLewa.getPosition().x, paletkaLewa.getPosition().y - 5));
			ruch_palet_lew.restart();
		}
		else if (out[0] <= 0.5 && paletkaLewa.getPosition().y < 520) {
			paletkaLewa.setPosition(sf::Vector2f(paletkaLewa.getPosition().x, paletkaLewa.getPosition().y + 5));
			ruch_palet_lew.restart();
		}
	}
}

void Game::AI_getdane() {
	if (getdan.getElapsedTime().asSeconds() > 0.1) {
		float x_diff_ball = (pilka.getPosition_x() - 30) / 10;
		float y_diff_ball = (pilka.getPosition_y() - paletkaLewa.getPosition().y-40)/5;
		dane[iterator][0] = x_diff_ball;
		dane[iterator][1] = y_diff_ball;

		neuralnet->set_inputs(dane[iterator]);
		outs[iterator] = neuralnet->forward_propagation();
		

		iterator++;
		if (iterator == 20) {
			iterator = 0;
		}
		getdan.restart();
	}

}

void Game::AI_learn() {
	if (odbicie == true) {
		for (int i = 0; i < dane.size(); i++) {
			
			neuralnet->set_inputs(dane[i]);
			neuralnet->forward_propagation();
			
			if (outs[i][0] > 0.5) {
				expected[i][0] = 0.7;
			}
			else {
				expected[i][0] = 0.3;
			}
			neuralnet->set_expoutputs(expected[i]);
			neuralnet->back_propagation();
		}
		iterator = 0;
		odbicie = false;
	}
	else if (punkt_zdobyty_enemy == true) {
		for (int i = 0; i < dane.size(); i++) {

			neuralnet->set_inputs(dane[i]);
			neuralnet->forward_propagation();
			if (outs[i][0] > 0.5) {
				expected[i][0] = 0;
			}
			else {
				expected[i][0] = 1;
			}
			neuralnet->set_expoutputs(expected[i]);
			neuralnet->back_propagation();
		}
		iterator = 0;
		punkt_zdobyty_enemy = false;
	}



}

void Game::AI_player() {
	if (ruch_palet_praw.getElapsedTime().asSeconds() > 0.02) {
		float x_diff_ball = (560-pilka.getPosition_x()) / 10;
		float y_diff_ball = (pilka.getPosition_y() - paletkaPrawa.getPosition().y-40)/5;

		vector_1d ins;
		ins.resize(2);
		ins[0] = x_diff_ball;
		ins[1] = y_diff_ball;

		neuralnetright->set_inputs(ins);

		vector_1d out = neuralnetright->forward_propagation();

		if (out[0] > 0.5 && paletkaPrawa.getPosition().y > 150) {
			paletkaPrawa.setPosition(sf::Vector2f(paletkaPrawa.getPosition().x, paletkaPrawa.getPosition().y - 5));
			ruch_palet_praw.restart();
		}
		else if (out[0] <= 0.5 && paletkaPrawa.getPosition().y < 520) {
			paletkaPrawa.setPosition(sf::Vector2f(paletkaPrawa.getPosition().x, paletkaPrawa.getPosition().y + 5));
			ruch_palet_praw.restart();
		}
	}

	if (getdane2.getElapsedTime().asSeconds() > 0.1) {
		float x_diff_ball = (560-pilka.getPosition_x()) / 10;
		float y_diff_ball = (pilka.getPosition_y() - paletkaPrawa.getPosition().y-40) / 5;
		daneright[iterator2][0] = x_diff_ball;
		daneright[iterator2][1] = y_diff_ball;

		neuralnetright->set_inputs(daneright[iterator2]);
		outsright[iterator2] = neuralnetright->forward_propagation();


		iterator2++;
		if (iterator2 == 20) {
			iterator2 = 0;
		}
		getdane2.restart();
	}

	if (odbicie2 == true) {
		for (int i = 0; i < daneright.size(); i++) {

			neuralnetright->set_inputs(daneright[i]);
			neuralnetright->forward_propagation();

			if (outsright[i][0] > 0.5) {
				expectedright[i][0] = 0.7;
			}
			else {
				expectedright[i][0] = 0.3;
			}
			neuralnetright->set_expoutputs(expectedright[i]);
			neuralnetright->back_propagation();
		}
		iterator2 = 0;
		odbicie2 = false;
	}
	else if (punkt_zdobyty_AI == true) {
		for (int i = 0; i < daneright.size(); i++) {

			neuralnetright->set_inputs(daneright[i]);
			neuralnetright->forward_propagation();
			if (outsright[i][0] > 0.5) {
				expectedright[i][0] = 0;
			}
			else {
				expectedright[i][0] = 1;
			}
			neuralnetright->set_expoutputs(expectedright[i]);
			neuralnetright->back_propagation();
		}
		iterator2 = 0;
		punkt_zdobyty_AI = false;
	}

}

void Game::reset() {
	pilka.reset();
	paletkaLewa.setPosition(sf::Vector2f(30, 350));
	paletkaPrawa.setPosition(sf::Vector2f(560, 350));
}

int main() {
	srand(time(NULL));

	sf::RenderWindow okno(sf::VideoMode(600, 600), "Pong");


	Neural_net neu_net(2, 20, 1, 2);
	Neural_net neu_net2(2, 20, 1, 2);

	Game gra(&neu_net,&neu_net2);
	
	
	while (okno.isOpen()) {

		sf::Event event;
		while (okno.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				okno.close();

			}
		}
		okno.clear();

		gra.gra(event,okno);
		
		
		okno.display();

	}
}