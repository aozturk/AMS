#pragma once

#include "AMS/Net/Socket.h"
#include "AMS/Types.h"
#include "AMS/IService.h"
#include "AMS/Config/Configurator.h"
#include "AMS/Reactor/IHandler.h"
#include "AMS/PubSub/IPubSub.h"

namespace AMS {
    template<typename T>
    class PubSub : public IPubSub
    {
    public:
        PubSub(Socket& socket)
            : m_socket(socket) {
        }
        ~PubSub(void) {
        }

        Socket& getSocket() {
            return m_socket;
        }

        void subscribe();
        void unsubscribe();

        IMsgObj* receive_message(SingleMessage& message);

        void send_message(IMsgObj& msg_obj);
        
    private:       

        Socket& m_socket;
        socket_type m_socktype;
    };

    template<typename T>
    void PubSub<T>::subscribe() {
        T obj;
        int id = obj.getMessageObjectId();
        std::ostringstream filter;
        filter << id << "-";
        m_socket.subscribeTopic(filter.str());
    }

    template<typename T>
    void PubSub<T>::unsubscribe() {
        T obj;
        int id = obj.getMessageObjectId();
        std::ostringstream filter;
        filter << id << "-";
        m_socket.unsubscribeTopic(filter.str());
    }

    template<typename T>
    IMsgObj* PubSub<T>::receive_message(SingleMessage& message) {  
        msgpack::unpacked msg;
        msgpack::unpack(&msg, ((char*)message.raw_data())+MSG_NAME_SIZE, message.size()-MSG_NAME_SIZE);
        msgpack::object obj = msg.get();
        // you can convert msg_obj to myclass directly
        T* msg_obj = new T;
        obj.convert(msg_obj);

        return msg_obj;
    }

    template<typename T>
    void PubSub<T>::send_message(IMsgObj& msg_obj) {
        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, dynamic_cast<T&>(msg_obj));

        char msg_name [MSG_NAME_SIZE] = {0};
        sprintf (msg_name, "%d-", msg_obj.getMessageObjectId());

        int totsize = sbuf.size() + MSG_NAME_SIZE;

        SingleMessage message(totsize);
        memcpy((char*) message.raw_data(), msg_name, MSG_NAME_SIZE);
        memcpy((char*) message.raw_data()+MSG_NAME_SIZE, sbuf.data(), sbuf.size());

        //  Send message to all subscribers        
        m_socket.send(message);
    }
}
