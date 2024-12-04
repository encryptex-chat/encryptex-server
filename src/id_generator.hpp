#pragma once
#include <boost/asio/experimental/coro.hpp>
#include <boost/cobalt/generator.hpp>
#include <generator>

namespace etex::details
{

class id_generator
{
    public:
    static auto get_id()
    {
        static uint64_t user_id = 0;
        user_id++;
        return user_id;
    }
};
}  // namespace etex::details