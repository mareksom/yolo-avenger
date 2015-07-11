#include "Run.h"

#include "HexBoard.h"
#include "TriangleBoard.h"

struct App
{
	typedef Hex::Board BoardType;

	std::string title() { return "Board - Avenger"; }

	void init(BoardType & board)
	{
		printf("init\n");
		board.addFieldAction("Action 1", [] (int x, int y) { printf("action1 %d %d\n", x, y); });
		board.addFieldAction("Action 2", [] (int x, int y) { printf("action2 %d %d\n", x, y); });
		board.addFieldAction("Action 3", [] (int x, int y) { printf("action3 %d %d\n", x, y); });
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
