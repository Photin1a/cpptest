#include "tcp_connect.h"

int EndPoint(const std::string &ip_address, const unsigned short port,
             boost::asio::ip::tcp::endpoint &end_point) {
  boost::system::error_code ec;
  auto ip = boost::asio::ip::address::from_string(ip_address, ec);
  if (ec.value() != 0) {
    std::cout << "Failed to parse the IP address. Error code " << ec.value()
              << ". " << ec.message() << std::endl;
    return ec.value();
  }
  end_point = boost::asio::ip::tcp::endpoint(ip, port);
  return 0;
}

Session::Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr, const std::function<void(const std::string &)> &ReadCallback)
    : socket_ptr_(socket_ptr), ReadCallback_(ReadCallback) {}

std::shared_ptr<boost::asio::ip::tcp::socket> &Session::Socket() {
  return socket_ptr_;
}

void Session::Start() { Read(); }

bool Session::IsCorrupt() { return corrupt_; }

void Session::Close() {
  boost::system::error_code err;
  socket_ptr_->close(err);
  corrupt_ = true;
}

bool Session::AddressMsg(const std::string &msg) {
  std::cout << "Client disconnected: "
            << socket_ptr_->remote_endpoint().address() << ":"
            << socket_ptr_->remote_endpoint().port() << "=>" << msg
            << std::endl;
}

void Session::Read() {
  std::shared_ptr<Session> ptr = shared_from_this(); // reference +1
  memset(read_buf_, 0, read_max_length);
  socket_ptr_->async_read_some(boost::asio::buffer(read_buf_, read_max_length),
                               std::bind(&Session::ReadHandler, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
}

void Session::ReadHandler(const boost::system::error_code &error,
                          size_t bytes_transfered) {
  if (error.value() == 2) { //断开连接
    std::cout << "Client disconnected: "
              << socket_ptr_->remote_endpoint().address() << ":"
              << socket_ptr_->remote_endpoint().port() << std::endl;
    Close();
    return;
  } else if (error.value() != 2 && error.value() != 0) { //发生错误
    std::cout << "[Warn] " << error.value() << " " << error.message()
              << std::endl;
    Close();
    return; //直接返回，不再进入读取循环
  } else {
    if (ReadCallback_ == nullptr)
      AddressMsg(std::string(read_buf_));
    else
      ReadCallback_(std::string(read_buf_));
    Read();
  }
}

void Session::Write(const std::string &buf) {
  auto self(shared_from_this()); // reference +1
  socket_ptr_->async_write_some(boost::asio::buffer(buf),
                                std::bind(&Session::WriteHandler, this,
                                          std::placeholders::_1,
                                          std::placeholders::_2));
}

void Session::WriteHandler(const boost::system::error_code &error,
                           size_t transfered) {
  if (error) {
    Close();
    std::cout << "Send Error, code  " << error.value() << ", message "
              << error.message() << std::endl;
    std::cout << "Session closed! \n ";
    return;
  }
}

TCPServer::TCPServer(int port, const std::function<void(const std::string&)> &SessionCallback)
    : acceptor_(io_service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      SessionCallback_(SessionCallback){
  work_ptr = std::make_shared<boost::asio::io_service::work>(io_service_);
  run_thread_timer_ =
      std::make_shared<boost::asio::deadline_timer>(io_service_);
  startAccept();
}

TCPServer::~TCPServer() { Stop(); };

void TCPServer::AsynRun(const int &until_time_mins) {
  if (until_time_mins != 0) {
    run_thread_timer_->expires_from_now(
        boost::posix_time::minutes(until_time_mins));
    run_thread_timer_->async_wait(
        [this](const boost::system::error_code &error) { Stop(); });
  }
  run_thread_ptr_ =
      std::make_shared<std::thread>([this]() { io_service_.run(); });
  run_thread_ptr_->detach();
}
void TCPServer::Run(const int &until_time_mins) {
  if (until_time_mins != 0) {
    run_thread_timer_->expires_from_now(
        boost::posix_time::minutes(until_time_mins));
    run_thread_timer_->async_wait(
        [this](const boost::system::error_code &error) { Stop(); });
  }
  io_service_.run();
}
void TCPServer::Stop() { io_service_.stop(); }

void TCPServer::startAccept() {
  std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr(
      new boost::asio::ip::tcp::socket(io_service_));
  acceptor_.async_accept((*socket_ptr),
                         [this, socket_ptr](boost::system::error_code ec) {
                           if (!ec) {
                             AddSession(socket_ptr);
                           }
                           startAccept();
                         });
}

void TCPServer::AddSession(
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr) {
  int num = 0;
  for (auto iter = sessions_ptr_.begin(); iter != (sessions_ptr_.end());) {
    if ((*iter)->IsCorrupt()) {
      iter = sessions_ptr_.erase(iter);
    } else
      iter++;
  }
  sessions_ptr_.push_back(
      std::make_shared<Session>(socket_ptr, SessionCallback_));
  sessions_ptr_.back()->Start();
  std::cout << "Client connected: " << socket_ptr->remote_endpoint()
            << std::endl;
}

void TCPServer::AsynSend(const std::shared_ptr<Session> &session, const std::string &Msg) {
  session->Write(Msg);
}

const std::shared_ptr<Session> &TCPServer::GetSession(const unsigned int &index) {
  if (index < GetSessionNum()) {
    return sessions_ptr_.at(index);
  }
  else return nullptr;
}

int TCPServer::GetSessionNum() { return sessions_ptr_.size(); }
