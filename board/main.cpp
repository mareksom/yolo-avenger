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

		board.addFieldSeparator();

		board.addFieldCheckButton("Check 1", [] (int x, int y, bool state) { printf("check1 %d %d %s\n", x, y, state ? "true" : "false"); });
		board.addFieldCheckButton("Check 2", [] (int x, int y, bool state) { printf("check2 %d %d %s\n", x, y, state ? "true" : "false"); });
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
