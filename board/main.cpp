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
