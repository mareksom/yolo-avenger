#pragma once
/* Funkcja do uruchomienia planszy. */

#include <gtkmm-3.0/gtkmm.h>

#include <bits/stdc++.h>

#include "Scene.h"

/* Controller -- klasa będąca pośrednikiem między planszą, a kodem użytkownika.
 * Musi zawierać:
 *   typ o nazwie BoardType -- typ planszy
 *   funkcję std::string title(); -- tytuł do wyświetlenia na pasku okna.
 *   funkcję void init(BoardType & board); -- inicjalizację planszy przed wyświetleniem jej
 *   funkcję void main(); -- kod, który zostanie wykonany po pokazaniu planszy
 *
 * W przyszłości tu będą dodawane metody, dzięki którym użytkownik będzie mógł kontrolować planszę.
 *
 * Funkcja Run() przejmuje kontrolę nad programem i nie wraca aż do momentu zamknięcia okna.
 * Aby móc wykonać swój kod w trakcie wykonywania planszy, należy umieścić go w metodzie main() kontrolera.
 */
template<typename Controller>
void Run()
{
	Controller controller;
	std::string title = controller.title();
	typedef typename Controller::BoardType Board;
	int myargc = 1;
	char ** myargv = new char*[2];
	myargv[0] = new char[title.length() + 1];
	for(int i = 0; i < (int) title.length(); i++)
		myargv[0][i] = title[i];
	myargv[0][title.length()] = 0;
	myargv[1] = nullptr;
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(myargc, myargv);

	Gtk::Window window;

	Board board;
	Scene<Board> scene(board);
	board.setScene(&scene);

	window.add(scene);
	scene.show();
	window.maximize();

	controller.init(board);

	std::thread thread([&controller] () { controller.main(); });
	thread.detach();

	app->run(window);

	delete[] myargv[0];
	delete[] myargv;
}
