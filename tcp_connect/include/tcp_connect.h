#ifndef TCP_CONNECT_H
#define TCP_CONNECT_H

#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <thread>

//用于生成端点：ip+port
int EndPoint(const std::string &ip_address, const unsigned short port,
             boost::asio::ip::tcp::endpoint &end_point);

class Session;
class TCPServer; 
class TCPServer {
public:
  //constructer 
  //default SessionCallback = nullptr, server will print to terminal when receive msg. 
  TCPServer(int port, const std::function<void(const std::string &)>& SessionCallback = nullptr);
  ~TCPServer();

  //Asynchronously, run the tcp-server until Stop() or until_time_mins. 
  //Server  will close when main-thread is closed! So you need to offer a clog in main-thread.
  //until_time_mins = 0 is  forever .
  void AsynRun(const int &until_time_mins = 0);

  //Run the tcp-server until until_time_mins.
  //Run() is a clog function.
  //until_time_mins = 0 is  forever .
  void Run(const int &until_time_mins = 0);

  //Quit all sessions and close the tcp-server.
  void Stop();

  //Asynchronously, send a msg. AsynSend does not clog.
  void AsynSend(const std::shared_ptr<Session>& session, const std::string &Msg);

  //Return a sesstion with index.
  const std::shared_ptr<Session>& GetSession(const unsigned int &index);

  //Get the num of current sesstions.
  int GetSessionNum();

  //Dot use it !
  void AddSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr);

private:
  void startAccept();

  const std::function<void(const std::string &)> SessionCallback_;
  std::vector<std::shared_ptr<Session>> sessions_ptr_;
  boost::asio::io_service io_service_;
  std::shared_ptr<boost::asio::io_service::work> work_ptr;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::shared_ptr<std::thread> run_thread_ptr_;
  std::shared_ptr<boost::asio::deadline_timer> run_thread_timer_;
};
//会话
class Session : public std::enable_shared_from_this<Session> {
  friend void TCPServer::AddSession(std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr);
public:
  Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr, const std::function<void(const std::string &)> &ReadCallback);
  ~Session() = default;
  std::shared_ptr<boost::asio::ip::tcp::socket> &Socket();
  bool IsCorrupt();
  void Write(const std::string &buf); //发送函数

protected:
  void Start();
  void Close();

private:
  bool AddressMsg(const std::string &msg); //在这里添加任务
  void Read();                             //读消息函数
  void ReadHandler(const boost::system::error_code &error,
                   size_t bytes_transfered);
  void WriteHandler(const boost::system::error_code &error,
                    size_t transfered); //发送结束之后触发

private:
  const std::function<void(const std::string &)> &ReadCallback_;
  std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr_;
  bool corrupt_ = false;
  enum { read_max_length = 10240 }; //最大报文接收大小 10k
  char read_buf_[read_max_length];
};
#endif