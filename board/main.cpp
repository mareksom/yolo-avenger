#include "Run.h"

#include "HexBoard.h"
#include "TriangleBoard.h"

struct App
{
	typedef Triangle::Board BoardType;
  BoardType* m_Board = nullptr;

	std::string title() { return "Board - Avenger"; }

	void init(BoardType & board)
	{
		printf("init\n");
    m_Board = &board;
    
    for (int i=0; i<10; i++)
      for (int j=0; j<10; j++)
      {
        m_Board -> operator () (i,j);
      }
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
