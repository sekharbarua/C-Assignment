#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <Windows.h> 
#include "time.h"
#include <stdio.h>
#include <string>
#include <chrono>
#include <conio.h>

using namespace std;

class OrderBase
{
private:
  std::string id_;
  int price_;
  int size_;

public:
  OrderBase() { /* Zero Argument Constructor */ };
  ~OrderBase() {}
  OrderBase(int size, int price, string id) : size_(size), price_(price), id_(id)
  {
    //Parameterised Constructor
  }
  OrderBase(const OrderBase& oB) :size_(oB.size_), price_(oB.price_), id_(oB.id_)
  {
    // Copy Constructor
  }
  OrderBase(OrderBase&& oB) noexcept : size_(std::move(oB.size_)), price_(std::move(oB.price_)), id_(std::move(oB.id_))
  {
    //Move Constructor
  }
  static OrderBase* make_order(int choice);  // Factory Method

  friend ostream & operator <<(ostream &dout, OrderBase& o)
  {
    dout << "Printing Order Detail" << endl;
    dout << "Order Id:- " << o.id_ << "\n" << "Order Price:- " << o.price_ << "\n" << "Order Size:- " << o.size_ << endl;
    return dout;
  }
};


class Order :public OrderBase
{
private:
  char side_; //'B' for Buy 'S' for Sell
  std::string symbol_;
public:
  Order() :OrderBase() {};
  Order(char side, string symbol, int size, int price, string id) :OrderBase(size, price, id), side_(side), symbol_(symbol) {};
  ~Order() {};
};

class AmendOrder :public OrderBase
{
public:
  AmendOrder(int size, int price, string id) :OrderBase(size, price, id) {}
  ~AmendOrder() {}
};

class PullOrder :public OrderBase
{
private:
  std::string _pOrderid;
public:
  PullOrder() {}
  PullOrder(std::string pId) : OrderBase(0, 0, pId), _pOrderid(pId) {}
  ~PullOrder() {}
};

OrderBase* OrderBase::make_order(int choice)
{
  int size; int price; std::string id;
  char side; std::string symbol;

  switch (choice)
  {
  case 1:
    std::cout << "****Amend Order Menu************" << endl;
    std::cout << "Please Enter the Size in numbers" << endl;
    std::cin >> size;
    std::cout << "Please Enter the Price in numbers" << endl;
    std::cin >> price;
    std::cout << "****Please Enter the OrderID****" << endl;
    std::cin >> id;
    return new AmendOrder(size, price, id);
    break;
  case 2:
    std::cout << "***Pull Order Menu******" << endl;
    std::cout << "Please Enter the OrderID " << endl;
    std::cin >> id;
    return new PullOrder(id);
    break;

  case 3:
  default:

    do
    {
      //system("CLS");
      std::cout << "***********************New Order Menu**************" << endl;
      std::cout << "Please Enter B - for Buy Order / S - for Sell Order" << endl;
      std::cin >> side;
      if (!isalpha(side))
      {
        std::cout << "Invalid Input - Press Any key to continue" << endl;
        _getch();
      }
    } while (side != 'B' && side != 'S');

    std::cout << "Please Enter the Symbol string" << endl;
    std::cin >> symbol;
    std::cout << "Please Enter the Size in Numbers" << endl;
    std::cin >> size;
    std::cout << "Please Enter the Price in Numbers" << endl;
    std::cin >> price;
    std::cout << "Please Enter the OrderID" << endl;
    std::cin >> id;
    return new Order(side, symbol, size, price, id);
    break;
  }
}

template <typename T>
class Queue
{
public:

  Queue()
  {
  }
  T pop()
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }
    auto item = queue_.front();
    queue_.pop();
    return item;
  }

  void pop(T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }
    item = queue_.front();
    queue_.pop();
  }

  void push(const T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    mlock.unlock();
    cond_.notify_one();
  }

  void push(T&& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(std::move(item));
    mlock.unlock();
    cond_.notify_one();
  }

  int size()
  {
    return int(queue_.size());
  }

  bool isEmpty()
  {
    return bool(queue_.empty());
  }

private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;
};

class Timer
{
public:
  static long long get_time_ms()
  {

    SYSTEMTIME t;
    GetLocalTime(&t);
    return t.wMilliseconds;
  }
};

class SlidingWindow
{
public:
  SlidingWindow()
  {
    this->lastSchedAction_ = std::chrono::system_clock::now();
  }
  const auto getLastSchedAction()
  {
    return lastSchedAction_;
  }
  void consumeOrder()
  {
    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastSchedAction_);
    while (int_ms.count() > 0)
    {
      int_ms--; // Sleeping logic if the all the order's are process in less than 1000 millisescond
    }
    this->lastSchedAction_ = std::chrono::system_clock::now();
  }
private:

  std::chrono::time_point<std::chrono::system_clock> lastSchedAction_;

};