#include <Message.h>
using namespace std;

int main()
{

  Queue<OrderBase*> queue;
  const int MAXQUEUESIZE = 400;
  const int WINDOW_SIZE = 100;
  for (int iCnt = 0; iCnt < MAXQUEUESIZE; iCnt++)
  {
    int randNum = (rand() % 3) + 1; // This will have input in the range from 1 - 3
    system("CLS");
    std::cout << "Entry for Order No - " << "\t" << iCnt + 1 << endl;
    queue.push(Order::make_order(randNum));
  }
  system("CLS");
  std::cout << "Size of Queue = " << queue.size() << std::endl;
  SlidingWindow sWindow = SlidingWindow();

  while (queue.isEmpty() == false)
  {
    for (int iCnt = 1; iCnt <= WINDOW_SIZE; iCnt++)
    {
      if (queue.isEmpty() == false) {
        std::cout << "***Processing Order***" << endl;
        OrderBase *mOb = std::move(queue.pop());
        std::cout << *mOb;
        if (iCnt%WINDOW_SIZE == 0)
        {
          std::cout << "*********************ACKNOWLEDGEMENT**********" << "\n" 
                    << "            ACK is received by Sender         " << "\n" 
                    << "***********************************************" << endl;
        }
      }
      else
      {
        break;
      }
    }
    sWindow.consumeOrder();

  }
  std::cout << "Transaction Order Processing Completed !!" << std::endl;
  _getch();
  return 0;
}