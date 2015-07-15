#include "Run.h"

#include "HexBoard.h"
#include "TriangleBoard.h"

bool tab[105][105];

struct App
{
	typedef Hex::Board BoardType;

	std::string title() { return "Board - Avenger"; }

	void init(BoardType & board)
	{
		printf("init\n");
		for(int i = 0; i < 20; i++)
		{
			for(int j = 0; j < 20; j++)
			{
				for(int k = 0; k < i + j; k++)
					board(i, j).addPopupAction("Action " + std::to_string(k), [i, j, k] () { printf("Akcja %d (%d, %d).\n", k, i, j); });
				board(i, j).addPopupSeparator();
				board(i, j).addPopupCheckAction("Bool", tab[i][j], [i, j] (bool stan) { printf("Bool (%d, %d) = %d = %d.\n", i, j, (int) stan, (int) tab[i][j]); });
			}
		}
		tab[5][5] = true;
	}

	void main()
	{
		printf("mejn\n");
	}
};

int main(int argc, char ** argv)
{
	Run<App>();
	return 0;
}
