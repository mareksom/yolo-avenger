#pragma once
/*
 * Field -- pole znajdujące się na planszy, jakiś bliżej niezidentyfikowany obiekt (pot. UFO),
 *          np. pojedynczy hex, albo pojedynczy trójkąt, albo wierzchołek grafu
 * Board -- plansza; jest zbiorem pól, jest odpowiedzialna za ich rozmieszczenie i rysowanie;
 *          planszę można wyświetlić, można przybliżać/oddalać, można zaznaczać elementy i inne bajery
 */

#include <bits/stdc++.h>

namespace Board {

/* Pole można pokolorować na dowolny kolor (podając RGB).
 * Pole trzyma swoje współrzędne (x, y) i umie siebie narysować
 * (aczkolwiek sposób rysowaniu musi ustalić z planszą, na której leży).
 */
class Field
{
public:
	Field(int x, int y) :
		mx(x), my(y)
		,r((double) rand() / RAND_MAX)
		,g((double) rand() / RAND_MAX)
		,b((double) rand() / RAND_MAX)
		,m_selected(false)
	{
	}

	/* Pobierz współrzędne pola */
	int x() const { return mx; }
	int y() const { return my; }

	/* Pobierz współrzędne pola jako string */
	const std::string coordinatesString() const
	{
		return "(" + std::to_string(mx) + ", " + std::to_string(my) + ")";
	}

	/* Pobierz kolor pola */
	void getColor(double & r, double & g, double & b)
	{
		r = this->r;
		g = this->g;
		b = this->b;
	}

	/* Ustaw kolor pola */
	void setColor(double r, double g, double b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

	/* Domyślnie pole rysuje siebie poprzez wypełnienie wszystkiego jednym kolorem.
	 * Plansza jest odpowiedzialna za obcięcie regionu po którym pole rysuje do kształtu pola.
	 */
	void draw(Cairo::RefPtr<Cairo::Context> context) const
	{
		context->save();
			if(selected())
				context->set_source_rgb(1, 1, 1);
			else
				context->set_source_rgb(r, g, b);
			context->paint();
		context->restore();
	}

	/* Zaznacz pole */
	void select()
	{
		m_selected = true;
	}

	/* Odznacz pole */
	void deselect()
	{
		m_selected = false;
	}

	/* Czy pole jest zaznaczone? */
	bool selected() const
	{
		return m_selected;
	}

private:
	int mx, my;
	double r, g, b;

	bool m_selected;
};

/* Plansza, która zawiera pola typu FieldType, i która sama jest typu BoardType.
 * Uwaga!!! możliwe, że BoardType != Board<FieldType, BoardType>, po to jest typ BoardType.
 * Być może BoardType jest klasą która dziedziczy z tej klasy np. tak:
 *   class DzieckoBoard : public Board<Field, DzieckoBoard> {};
 * Chodzi o to, żeby BoardType było typem planszy z samego dołu łańcucha pokarmowego.
 */
template<typename FieldType, typename BoardType>
class Board
{
public:
	Board() : m_fieldHovered(nullptr), scene(nullptr)
	{
	}

	/* Przypisuje planszę do sceny (tam gdzie plansza będzie wyświetlana). */
	void setScene(Scene<BoardType> * scene)
	{
		this->scene = scene;
	}

	/* Daj referencję do pola o współrzędnych (x, y).
	 * Jeśli pole nie istnieje, zostanie stworzone. */
	FieldType & operator () (int x, int y)
	{
		auto it = fields.find(std::make_pair(x, y));
		if(it == fields.end())
			return fields.emplace(std::make_pair(x, y), FieldType(x, y)).first->second;
		return it->second;
	}

	/* Daj wskaźnik do pola o współrzędnych (x, y).
	 * Jeśli pole nie istnieje, zostanie zwrócony nullptr. */
	FieldType * fieldPtr(int x, int y)
	{
		auto it = fields.find(std::make_pair(x, y));
		if(it == fields.end())
			return nullptr;
		return &it->second;
	}

	/* Daj referencję do pola, które znajduje się pod współrzędnymi (x, y) sceny.
	 * Jeśli pole nie istnieje, zostanie stworzone. */
	FieldType & getAt(double x, double y)
	{
		auto coordinates = toCoords(x, y);
		return operator () (coordinates.first, coordinates.second);
	}

	/* Daj wskaźnik do pola, które znajduje się pod współrzędnymi (x, y) sceny.
	 * Jeśli pole nie istnieje, zostanie zwrócony nullptr. */
	FieldType * getAtPtr(double x, double y)
	{
		auto coordinates = toCoords(x, y);
		return fieldPtr(coordinates.first, coordinates.second);
	}

	/* Metoda wywoływana przez scenę, mówiąca, że element pod współrzędnymi (x, y) sceny
	 * został najechany myszką. */
	void hover_event(double x, double y)
	{
		hoverField(getAtPtr(x, y));
	}

	/* Oznaczenie pola field jako najechanego myszką.
	 * Jeśli field == nullptr, to oznaczenie, że żadne pole nie jest najechane. */
	void hoverField(FieldType * field)
	{
		std::swap(field, m_fieldHovered);
		if(field)
			invalidateField(*field);
		if(m_fieldHovered)
			invalidateField(*m_fieldHovered);
	}

	/* Zwróć wskaźnik na pole najechane myszką.
	 * Jeśli żadne pole nie jest najechane, zwraca nullptr. */
	FieldType * fieldHovered() { return m_fieldHovered; }

	/* Wyczyść zaznaczenie -> po tej operacji żadne pole nie będzie zaznaczone. */
	void clearSelection()
	{
		for(auto f : selectedFields)
		{
			f->deselect();
			invalidateField(*f);
		}
		selectedFields.clear();
	}

	/* Narysuj planszę w podanym kontekście.
	 * Kontekst będzie przycięty do wymiarów ekranu. */
	void draw(Cairo::RefPtr<Cairo::Context> context)
	{
		double x, y, width, height;
		context->get_clip_extents(x, y, width, height);
		width -= x;
		height -= y;
		forEachFieldInRect(
			x, y, width, height,
			[this, context] (FieldType & f) {
				drawField(f, context);
			}
		);
		if(m_fieldHovered)
			drawField(*m_fieldHovered, context);
	}

	/* Dodaj pola nachodzące na prostokąt Rect(x, y, width, height) do zaznaczenia. */
	void addSelection(double x, double y, double width, double height)
	{
		forEachFieldInRectExact(
			x, y, width, height,
			[this] (FieldType & f) {
				addToSelection(&f);
				invalidateField(f);
			}
		);
	}

	/* Wywal pola nachodzące na prostokąt Rect(x, y, width, height) z zaznaczenia. */
	void removeSelection(double x, double y, double width, double height)
	{
		forEachFieldInRectExact(
			x, y, width, height,
			[this] (FieldType & f) {
				removeFromSelection(&f);
				invalidateField(f);
			}
		);
	}

	/* Ustaw zaznaczenie na pola nachodzące na prostokąt Rect(x, y, width, height). */
	void setSelection(double x, double y, double width, double height)
	{
		clearSelection();
		addSelection(x, y, width, height);
	}

protected:
	/* Dodaj pole field do zaznaczenia. */
	void addToSelection(FieldType * field)
	{
		field->select();
		selectedFields.insert(field);
	}

	/* Usuń pole field z zaznaczenia. */
	void removeFromSelection(FieldType * field)
	{
		field->deselect();
		selectedFields.erase(field);
	}

	/* Wywołaj funkcję f dla wszystkich (i tylko tych) pól, które nachodzą na prostokąt Rect(x, y, width, height). */
	void forEachFieldInRectExact(double x, double y, double width, double height, std::function<void(FieldType&)> f)
	{
		forEachFieldInRect(x, y, width, height,
			[this, f, x, y, width, height] (FieldType & field) {
				if(isFieldInsideRect(field, x, y, width, height))
					f(field);
			}
		);
	}

	/* Wywołaj funkcję f dla wszystkich (i tylko tych) pól, które całkowicie zawierają się w prostokącie Rect(x, y, width, height). */
	void forEachFieldEntirelyInRect(double x, double y, double width, double height, std::function<void(FieldType&)> f)
	{
		forEachFieldInRect(x, y, width, height,
			[this, f, x, y, width, height] (FieldType & field) {
				if(isFieldEntirelyInsideRect(field, x, y, width, height))
					f(field);
			}
		);
	}

	/* Narysuj od nowa całą scenę. */
	void invalidate() { scene->invalidate(); }
	/* Narysuj od nowa prostokąt Rect(x, y, width, height) na scenie. */
	void invalidateArea(double x, double y, double width, double height) { scene->invalidateArea(x, y, width, height); }

	/* Zamień współrzędne sceny na współrzędne pól. */
	virtual std::pair<int, int> toCoords(double x, double y) = 0;
	/* Narysuj pole zgodnie w dobrym miejscu sceny. */
	virtual void drawField(const FieldType & field, Cairo::RefPtr<Cairo::Context> context) = 0;
	/* Wywołaj funkcję f dla każdego pola, które nachodzi na prostokąt Rect(x, y, width, height).
	 * UWAGA!! Dopuszczalne jest aby f została wywołana dla jakichś pól poza danym prostokątem.
	 * Funkcję należy zoptymalizować, aby działała jak najszybciej, a nie, żeby wywoływała się
	 * dla jak najmniejszej liczby pól poza prostokątem. */
	virtual void forEachFieldInRect(double x, double y, double width, double height, std::function<void(FieldType&)> f) = 0;
	/* Czy pole nachodzi na prostokąt Rect(x, y, width, height)? */
	virtual bool isFieldInsideRect(const FieldType & field, double x, double y, double width, double height) = 0;
	/* Czy pole zawiera się całkowicie w prostokącie Rect(x, y, width, height)? */
	virtual bool isFieldEntirelyInsideRect(const FieldType & field, double x, double y, double width, double height) = 0;
	/* Narysuj od nowa pole na scenie. */
	virtual void invalidateField(const FieldType & field) = 0;

private:
	Scene<BoardType> * scene;

	std::map<
		std::pair<int, int>,
		FieldType
	> fields;

	FieldType * m_fieldHovered;
	std::unordered_set<FieldType*> selectedFields;
};

} // namespace Board
