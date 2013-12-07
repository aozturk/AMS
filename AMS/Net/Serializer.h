#pragma once

#include "AMS/Helper.h"

namespace AMS {
    template<typename T>
    class Serializer {
    public:
        Serializer() {
        }
        ~Serializer() {}

        void serialize() {
            // you can serialize user-defined class directly
            msgpack::pack(m_sbuf, m_obj);
        }

        void deserialize(size_t nbytes) {
            // specify the number of bytes actually copied
            m_pac.buffer_consumed(nbytes);

            msgpack::unpacked result;
            // repeat next() until it returns false
            while(m_pac.next(&result)) {
                // do some with the object with the zone
                msgpack::object obj = result.get();

                // you can convert object to user-defined class directly
                obj.convert(&m_obj);
            }
            if (m_pac.buffer_capacity() < MSGPACK_SBUFFER_INIT_SIZE) {
                // reserve buffer. at least 'size' bytes of capacity will be ready
                m_pac.reserve_buffer();
            }
        }
        // user-defined class object
        T& data() {
            return m_obj;
        }
        // serialization buffer data
        const char* sbuf_data() {
            return m_sbuf.data();
        }
        // serialization buffer size
        size_t sbuf_size() {
            return m_sbuf.size();
        }
        // overwrites to serialization buffer
        void write(const char* buf, unsigned int len) {
            m_sbuf.write(buf, len);
        }
        // deserialiaztion (unpacker) buffer
        char* pac_buffer() {
            return m_pac.buffer();
        }
        // deserialiaztion (unpacker) buffer size
        size_t pac_size() {
            return m_pac.buffer_capacity();
        }

    private:
        msgpack::sbuffer m_sbuf;
        T m_obj;
        msgpack::unpacker m_pac;
    };
}