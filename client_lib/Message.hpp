#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <string_view>
#include <vector>

#include <msgpack.hpp>

enum class MessageId : std::uint32_t {
    LoginRequest = 0,
    LoginReply,
    FoodListRequest,
    FoodListReply,
    OrderRequest,
    OrderReply,
    PayRequest,
    PayReply,
    OrderArrivedRequest,
    OrderArrivedReply,
    NotificationOrders,
    CompleteFoodRequest,
    CompleteFoodReply,
    FoodChangeRequest,
    FoodChangeReply,
    OrderStatusChangeRequest,
    OrderStatusChangeReply
};
MSGPACK_ADD_ENUM(MessageId);

inline std::string_view to_string(MessageId id) {
    switch (id) {
    case MessageId::LoginRequest:
        return "LoginRequest";
    case MessageId::LoginReply:
        return "LoginReply";
    case MessageId::FoodListRequest:
        return "FoodListRequest";
    case MessageId::FoodListReply:
        return "FoodListReply";
    case MessageId::OrderRequest:
        return "OrderRequest";
    case MessageId::OrderReply:
        return "OrderReply";
    case MessageId::PayRequest:
        return "PayRequest";
    case MessageId::PayReply:
        return "PayReply";
    case MessageId::OrderArrivedRequest:
        return "OrderArrivedRequest";
    case MessageId::OrderArrivedReply:
        return "OrderArrivedReply";
    case MessageId::NotificationOrders:
        return "NotificationOrders";
    case MessageId::CompleteFoodRequest:
        return "CompleteFoodRequest";
    case MessageId::CompleteFoodReply:
        return "CompleteFoodReply";
    case MessageId::FoodChangeRequest:
        return "FoodChangeRequest";
    case MessageId::FoodChangeReply:
        return "FoodChangeReply";
    case MessageId::OrderStatusChangeRequest:
        return "OrderStatusChangeRequest";
    case MessageId::OrderStatusChangeReply:
        return "OrderStatusChangeReply";
    }
    return "<INVALID>";
}

enum class OrderStatus : std::uint32_t { Pending, InProgress, Completed, Payed, PayIntent };
MSGPACK_ADD_ENUM(OrderStatus);

struct FoodContains {
    std::uint32_t FoodId;
    std::string FoodName;
    std::uint32_t FoodPrice;
    std::uint32_t Amount;

    MSGPACK_DEFINE(FoodId, FoodName, FoodPrice, Amount);
};

struct FoodAmount {
    std::uint32_t FoodId;
    std::uint32_t Amount;

    MSGPACK_DEFINE(FoodId, Amount);
};
struct Food {
    FoodContains FoodData;
    bool Visible;

    MSGPACK_DEFINE(FoodData, Visible);
};

struct Delta {
    std::vector<Food> ModifiedFoods;
    std::vector<Food> CreatedFoods;

    MSGPACK_DEFINE(ModifiedFoods, CreatedFoods);
};

struct Orders {

    std::uint64_t OrderId;
    std::string TableId;
    std::vector<FoodContains> OrderedFoods;
    std::uint64_t OrderedDate;
    OrderStatus Status;

    MSGPACK_DEFINE(OrderId, TableId, OrderedFoods, OrderedDate, Status);
};

struct MessageHeader {
    MessageId id;

    MSGPACK_DEFINE(id);
};

struct Message {
    virtual ~Message();

    virtual MessageId id() const = 0;

    MSGPACK_DEFINE();
};

enum class LoginStatus : std::uint32_t { Ok = 0, Error };

MSGPACK_ADD_ENUM(LoginStatus);

enum class PayStatus : std::uint32_t { Success, Failed };

MSGPACK_ADD_ENUM(PayStatus);

enum class FoodChangeStatus : std::uint32_t { Success, Failed };
MSGPACK_ADD_ENUM(FoodChangeStatus);

enum class ReplyStatus : std::uint32_t { Success, Failed };
MSGPACK_ADD_ENUM(ReplyStatus);

struct LoginRequestMessage final : Message {
    LoginRequestMessage() = default;
    LoginRequestMessage(const std::string &name, const std::string &password, const bool &isAdmin)
        : Name(name), Password(password), IsAdmin(isAdmin) {
    }

    MessageId id() const final {
        return MessageId::LoginRequest;
    }

    std::string Name;
    std::string Password;
    bool IsAdmin;

    MSGPACK_DEFINE(Name, Password, IsAdmin);
};

struct LoginReplyMessage final : Message {
    LoginReplyMessage() = default;
    LoginReplyMessage(const LoginStatus &status) : Status(status) {
    }

    MessageId id() const final {
        return MessageId::LoginReply;
    }

    LoginStatus Status;

    MSGPACK_DEFINE(Status);
};

struct FoodListRequestMessage final : Message {
    FoodListRequestMessage() = default;

    MessageId id() const final {
        return MessageId::FoodListRequest;
    }
};

struct FoodListReplyMessage final : Message {
    FoodListReplyMessage() = default;
    FoodListReplyMessage(const std::vector<FoodContains> &foods) : Foods(foods) {
    }

    MessageId id() const final {
        return MessageId::FoodListReply;
    }

    std::vector<FoodContains> Foods;

    MSGPACK_DEFINE(Foods);
};

struct OrderRequestMessage final : Message {
    OrderRequestMessage() = default;
    OrderRequestMessage(const std::vector<FoodAmount> &orderedFoods) : OrderedFoods(orderedFoods) {
    }
    MessageId id() const final {
        return MessageId::OrderRequest;
    }

    std::vector<FoodAmount> OrderedFoods;

    MSGPACK_DEFINE(OrderedFoods);
};

struct OrderReplyMessage final : Message {
    OrderReplyMessage() = default;
    OrderReplyMessage(std::uint64_t orderId, const std::vector<FoodContains> &orderedFoods)
        : OrderId(orderId), OrderedFoods(orderedFoods) {
    }

    MessageId id() const final {
        return MessageId::OrderReply;
    }

    std::uint64_t OrderId;
    std::vector<FoodContains> OrderedFoods;

    MSGPACK_DEFINE(OrderId, OrderedFoods);
};

struct PayRequestMessage final : Message {
    PayRequestMessage() = default;
    PayRequestMessage(std::uint64_t orderId) : OrderId(orderId) {
    }

    MessageId id() const final {
        return MessageId::PayRequest;
    }

    std::uint64_t OrderId;

    MSGPACK_DEFINE(OrderId);
};

struct PayReplyMessage final : Message {
    PayReplyMessage() = default;
    PayReplyMessage(PayStatus status) : Status(status) {
    }

    MessageId id() const final {
        return MessageId::PayReply;
    }
    PayStatus Status;

    MSGPACK_DEFINE(Status);
};
struct CompleteFoodRequestMessage final : Message {
    CompleteFoodRequestMessage() = default;

    MessageId id() const final {
        return MessageId::CompleteFoodRequest;
    }
};

struct CompleteFoodReplyMesage final : Message {
    CompleteFoodReplyMesage() = default;

    CompleteFoodReplyMesage(std::vector<Food> food) : Foods(food) {
    }

    MessageId id() const final {
        return MessageId::CompleteFoodReply;
    }

    std::vector<Food> Foods;

    MSGPACK_DEFINE(Foods);
};

struct FoodChangeRequestMessage final : Message {
    FoodChangeRequestMessage() = default;

    FoodChangeRequestMessage(Delta delta) : Changes(delta) {
    }

    MessageId id() const final {
        return MessageId::FoodChangeRequest;
    }
    Delta Changes;

    MSGPACK_DEFINE(Changes);
};

struct FoodChangeReplyMessage final : Message {
    FoodChangeReplyMessage() = default;

    FoodChangeReplyMessage(FoodChangeStatus status) : Status(status) {
    }
    MessageId id() const final {
        return MessageId::FoodChangeReply;
    }

    FoodChangeStatus Status;

    MSGPACK_DEFINE(Status);
};
struct OrderStatusChangeRequestMessage final : Message {
    OrderStatusChangeRequestMessage() = default;

    OrderStatusChangeRequestMessage(std::uint64_t orderId, OrderStatus status) : OrderId(orderId), Status(status) {
    }
    MessageId id() const final {
        return MessageId::OrderStatusChangeRequest;
    }

    std::uint64_t OrderId;
    OrderStatus Status;

    MSGPACK_DEFINE(OrderId, Status);
};
struct OrderStatusChangeReplyMessage final : Message {
    OrderStatusChangeReplyMessage() = default;

    OrderStatusChangeReplyMessage(std::uint64_t orderId, ReplyStatus status, OrderStatus newStatus, std::uint64_t date)
        : OrderId(orderId), Status(status), NewStatus(newStatus), Date(date) {
    }
    MessageId id() const final {
        return MessageId::OrderStatusChangeReply;
    }

    std::uint64_t OrderId;
    ReplyStatus Status;
    OrderStatus NewStatus;
    std::uint64_t Date;

    MSGPACK_DEFINE(OrderId, Status, NewStatus, Date);
};

struct OrderArrivedRequestMessage final : Message {
    OrderArrivedRequestMessage() = default;

    OrderArrivedRequestMessage(std::uint64_t fromDate, std::uint64_t toDate) : FromDate(fromDate), ToDate(toDate) {
    }

    MessageId id() const final {
        return MessageId::OrderArrivedRequest;
    }

    std::uint64_t FromDate;
    std::uint64_t ToDate;

    MSGPACK_DEFINE(FromDate, ToDate);
};

struct OrderArrivedReplyMessage final : Message {
    OrderArrivedReplyMessage() = default;

    OrderArrivedReplyMessage(std::vector<Orders> orders) : Orders(orders) {
    }

    MessageId id() const final {
        return MessageId::OrderArrivedReply;
    }

    std::vector<Orders> Orders;

    MSGPACK_DEFINE(Orders)
};

struct NotificationOrdersMessage final : Message {
    NotificationOrdersMessage() = default;

    NotificationOrdersMessage(Orders order) : Order(order) {
    }

    MessageId id() const final {
        return MessageId::NotificationOrders;
    }

    Orders Order;

    MSGPACK_DEFINE(Order);
};

#endif //! MESSAGE_HPP